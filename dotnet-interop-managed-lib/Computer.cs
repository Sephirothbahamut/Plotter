using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using LHM = LibreHardwareMonitor.Hardware;

namespace dotnet_interop_net_lib
	{
	class InvalidIdentifierException : Exception { public InvalidIdentifierException(string identifier) : base("Identifier [" + identifier + "] is not valid.") { } }
	class InvalidIdentifierInSensorException : Exception { public InvalidIdentifierInSensorException(string identifier, string sensor_id) : base("Trying to search identifier [" + identifier + "] inside sensor." + sensor_id + ".") { } }
	class IdentifierNotFoundException : Exception { public IdentifierNotFoundException(string identifier, string parent_id) : base("Cannot find identifier [" + identifier + "] inside " + parent_id + ".") { } }
	class SensorNotFoundException : Exception { public SensorNotFoundException(string identifier) : base("Cannot find sensor [" + identifier + "]. Is it an hardware?") { } }
	class HardwareNotFoundException : Exception { public HardwareNotFoundException(string identifier) : base("Cannot find hardware [" + identifier + "]. Is it a sensor?") { } }
	class IncompatibleSensorsException : Exception { public IncompatibleSensorsException(Sensor a, Sensor b) : base("Sensors [" + a.identifier + "] and [" + b.identifier + "] are not compatible.") { } }

	public abstract class ComputerComponent
		{
		public abstract string name { get; }
		public abstract string identifier { get; }
		public abstract string to_json(int indent = 0);
		}

	public abstract class Sensor : ComputerComponent
		{
		abstract public LHM.SensorType type { get; }
		public string value_string
			{
			get
				{
				switch (type)
					{
					case LHM.SensorType.Data: return Utils.ValuesConversions.dataGB(value);
					case LHM.SensorType.SmallData: return Utils.ValuesConversions.dataMB(value);
					case LHM.SensorType.Fan: return Utils.ValuesConversions.fan(value);
					case LHM.SensorType.Level:
					case LHM.SensorType.Control:
					case LHM.SensorType.Load: return Utils.ValuesConversions.load(value);
					case LHM.SensorType.Temperature: return Utils.ValuesConversions.temperature(value);
					default: return "TODO" + (value.HasValue ? value.Value.ToString() : "");
					}
				}
			}

		public abstract float? value { get; }
		public abstract float? min { get; }
		public abstract float? max { get; }

		public override string to_json(int indent = 0)
			{
			return Utils.s.indent(indent) + "{ " +
			"\"name\": " + Utils.s.uniform_length("\"" + name + "\"", 30) +
			", \"id\": " + Utils.s.uniform_length("\"" + identifier + "\"", 30) +
			", \"type\": " + Utils.s.uniform_length("\"" + type + "\"", 15) + "" +
			", \"value\": " + Utils.s.uniform_length("\"" + (value.HasValue ? value.Value.ToString() : "N/A") + "\"", 15) + "}";
			}
		}

	class SensorCombiner : Sensor
		{
		private Sensor a = null, b = null;
		public SensorCombiner(Sensor a, Sensor b)
			{
			if (a.type != b.type &&
				!((a.type == LHM.SensorType.Data || a.type == LHM.SensorType.SmallData) && (b.type == LHM.SensorType.Data || b.type == LHM.SensorType.SmallData)))
				{ throw new IncompatibleSensorsException(a, b); }
			this.a = a; this.b = b;
			}

		public override LHM.SensorType type { get { return a.type; } }

		public override string name { get { return a.name + " + " + b.name; } }
		public override string identifier { get { return "{" + a.identifier + "+" + b.identifier + "}"; } }

		public override float? value { get { return a.value + b.value; } }
		public override float? min { get { return a.min + b.min; } }
		public override float? max { get { return a.max + b.max; } }
		}

	class SensorDirect : Sensor
		{
		private LHM.ISensor _this = null;
		public SensorDirect(LHM.ISensor lhm_sensor) { this._this = lhm_sensor; }

		public override string name { get { return _this.Name; } }
		public override string identifier { get { return _this.Identifier.ToString(); } }
		public int index { get { return _this.Index; } }

		public override LHM.SensorType type { get { return _this.SensorType; } }

		public override float? value { get { return _this.Value; } }
		public override float? min { get { return _this.Min; } }
		public override float? max { get { return _this.Max; } }
		}

	public class Hardware : ComputerComponent
		{
		private LHM.IHardware _this = null;
		public Hardware(LHM.IHardware lhm_hardware) { this._this = lhm_hardware; }

		public override string name { get { return _this.Name; } }
		public override string identifier { get { return _this.Identifier.ToString(); } }

		public LHM.HardwareType type { get { return _this.HardwareType; } }

		public int sub_hardware_size { get { return _this.SubHardware.Length; } }
		public Utils.IndexedPropertyReadOnly<int, Hardware> sub_hardware
			{ get { return new Utils.IndexedPropertyReadOnly<int, Hardware>(index => { return new Hardware(_this.SubHardware[index]); }); } }

		public int sensors_size { get { return _this.Sensors.Length; } }
		public Utils.IndexedPropertyReadOnly<int, Sensor> sensors
			{ get { return new Utils.IndexedPropertyReadOnly<int, Sensor>(index => { return new SensorDirect(_this.Sensors[index]); }); } }

		public void update() { _this.Update(); foreach (LHM.IHardware lhm_hardware in _this.SubHardware) { new Hardware(lhm_hardware).update(); } }

		public override string to_json(int indent = 0)
			{
			update();
			string ret = Utils.s.indent(indent) + "{ " +
			"\"name\": \"" + name + "\"" +
			", \"id\": \"" + identifier + "\"" +
			", \"type\": \"" + type + "\"";
			if (_this.Sensors.Length > 0)
				{
				ret += ",\n" +
				Utils.s.indent(indent) + "\"sensor\": \n" +
				Utils.s.indent(indent + 1) + "[\n";
				for (int i = 0; i < sensors_size - 1; i++) { ret += sensors[i].to_json(indent + 2) + ",\n"; }
				ret += sensors[sensors_size - 1].to_json(indent + 2) + "\n" +
				Utils.s.indent(indent + 1) + "]";
				}
			if (_this.SubHardware.Length > 0)
				{
				ret += ",\n" +
				Utils.s.indent(indent) + "\"hardware\": \n" +
				Utils.s.indent(indent + 1) + "[\n";
				for (int i = 0; i < sub_hardware_size - 1; i++) { ret += sub_hardware[i].to_json(indent + 2) + ",\n"; }
				ret += sub_hardware[sub_hardware_size - 1].to_json(indent + 2) + "\n" +
				Utils.s.indent(indent + 1) + "]";
				}
			ret += "\n" + Utils.s.indent(indent) + "}";
			return ret;
			}
		}

	public class Computer : IDisposable
		{
		private LHM.Computer _this = null;

		public Computer()
			{
			_this = new LHM.Computer() { IsCpuEnabled = true, IsGpuEnabled = true, IsMemoryEnabled = true, IsMotherboardEnabled = true, IsControllerEnabled = true, IsStorageEnabled = true, IsNetworkEnabled = true };
			_this.Open();
			}

		public void update() { foreach (LHM.IHardware lhm_hardware in _this.Hardware) { new Hardware(lhm_hardware).update(); } }

		private Sensor get_sensor_in_hardware(string s, LHM.IHardware hardware)
			{
			Sensor found = null;
			foreach (var sensor in hardware.Sensors) { if (sensor.Identifier.ToString().Equals(s)) { return new SensorDirect(sensor); } }
			foreach (var sub_hardware in hardware.SubHardware) { found = get_sensor_in_hardware(s, sub_hardware); if (found != null) { return found; } }
			return null;
			}
		private Hardware get_hardware_in_hardware(string s, LHM.IHardware hardware)
			{
			if (hardware.Identifier.ToString().Equals(s)) { return new Hardware(hardware); }
			Hardware found = null;
			foreach (var sub_hardware in hardware.SubHardware) { found = get_hardware_in_hardware(s, sub_hardware); if (found != null) { return found; } }
			return null;
			}
		private ComputerComponent get_in_hardware(string s, LHM.IHardware hardware)
			{
			if (hardware.Identifier.ToString().Equals(s)) { return new Hardware(hardware); }
			ComputerComponent found = null;
			foreach (var sensor in hardware.Sensors) { if (sensor.Identifier.ToString().Equals(s)) { return new SensorDirect(sensor); } }
			foreach (var sub_hardware in hardware.SubHardware) { found = get_in_hardware(s, sub_hardware); if (found != null) { return found; } }
			return null;
			}

		public Sensor get_sensor(string s)
			{
			Sensor found = null;
			foreach (var hardware in _this.Hardware) { found = get_sensor_in_hardware(s, hardware); if (found != null) { return found; } }
			throw new SensorNotFoundException(s);
			}
		public Hardware get_hardware(string s)
			{
			Hardware found = null;
			foreach (var hardware in _this.Hardware) { found = get_hardware_in_hardware(s, hardware); if (found != null) { return found; } }
			throw new HardwareNotFoundException(s);
			}
		public ComputerComponent get(string s)
			{
			ComputerComponent found = null;
			foreach (var hardware in _this.Hardware) { found = get_in_hardware(s, hardware); if (found != null) { return found; } }
			throw new HardwareNotFoundException(s);
			}

		public string to_json(int indent = 0)
			{
			string ret = Utils.s.indent(indent) + "{ " +
			Utils.s.indent(indent) + "\"hardware\": \n" +
			Utils.s.indent(indent + 1) + "[\n";
			if (hardware_size > 0)
				{
				for (int i = 0; i < hardware_size - 1; i++) { ret += hardware[i].to_json(indent + 2) + ",\n"; }
				ret += hardware[hardware_size - 1].to_json(indent + 2) + "\n";
				}
			ret += Utils.s.indent(indent + 1) + "]\n" +
			Utils.s.indent(indent) + "}";
			return ret;
			}


		public int hardware_size { get { return _this.Hardware.Count; } }
		public Utils.IndexedPropertyReadOnly<int, Hardware> hardware
			{ get { return new Utils.IndexedPropertyReadOnly<int, Hardware>(index => { return new Hardware(_this.Hardware[index]); }); } }


		public void WriteToConsole(string message)
			{
			Console.WriteLine(message);
			}

		// DESTRUCTOR
		private bool disposed = false;
		~Computer() => Dispose(false);
		protected virtual void Dispose(bool disposing)
			{
			if (disposed) { return; }
			if (disposing) { }
			_this?.Close(); _this = null;

			disposed = true;
			}
		public void Dispose() { Dispose(disposing: true); GC.SuppressFinalize(this); }
		}
	}
