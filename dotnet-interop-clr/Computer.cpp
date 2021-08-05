#include "Computer.h"
#include <gcroot.h>
#include <msclr/marshal_cppstd.h>
#include <optional>
namespace LHM = LibreHardwareMonitor::Hardware;

sensor_type sensor_type_conversion(LHM::SensorType type)
	{
	switch (type)
		{
			case LibreHardwareMonitor::Hardware::SensorType::Voltage:  	  return sensor_type::voltage;
			case LibreHardwareMonitor::Hardware::SensorType::Clock:		  return sensor_type::clock;
			case LibreHardwareMonitor::Hardware::SensorType::Temperature: return sensor_type::temperature;
			case LibreHardwareMonitor::Hardware::SensorType::Load:		  return sensor_type::load;
			case LibreHardwareMonitor::Hardware::SensorType::Frequency:	  return sensor_type::frequency;
			case LibreHardwareMonitor::Hardware::SensorType::Fan:		  return sensor_type::fan;
			case LibreHardwareMonitor::Hardware::SensorType::Flow:		  return sensor_type::flow;
			case LibreHardwareMonitor::Hardware::SensorType::Control:	  return sensor_type::control;
			case LibreHardwareMonitor::Hardware::SensorType::Level:		  return sensor_type::level;
			case LibreHardwareMonitor::Hardware::SensorType::Factor:	  return sensor_type::factor;
			case LibreHardwareMonitor::Hardware::SensorType::Power:		  return sensor_type::power;
			case LibreHardwareMonitor::Hardware::SensorType::Data:		  return sensor_type::data;
			case LibreHardwareMonitor::Hardware::SensorType::SmallData:	  return sensor_type::smalldata;
			case LibreHardwareMonitor::Hardware::SensorType::Throughput:  return sensor_type::throughput;
			default: 													  return sensor_type::unknown;
		}
	}
std::string sensor_type_to_string(sensor_type type)
	{
	switch (type)
		{
			case sensor_type::voltage:			return "voltage";
			case sensor_type::clock:			return "clock";
			case sensor_type::temperature:		return "temperature";
			case sensor_type::load:				return "load";
			case sensor_type::frequency:		return "frequency";
			case sensor_type::fan:				return "fan";
			case sensor_type::flow:				return "flow";
			case sensor_type::control:			return "control";
			case sensor_type::level:			return "level";
			case sensor_type::factor:			return "factor";
			case sensor_type::power:			return "power";
			case sensor_type::data:				return "data";
			case sensor_type::smalldata:		return "smalldata";
			case sensor_type::throughput:		return "throughput";
			case sensor_type::unknown: default: return "Unknown";
		}
	}

std::string indent(uint8_t depth)
	{
	std::string ret = "";
	for (uint8_t i = 0; i < depth; i++) { ret += "    "; }
	return ret;
	}

LHM::ISensor^ get_sensor(LHM::IHardware^ hardware, System::String^ identifier)
	{
	for each (auto h in hardware->SubHardware)
		{
		LHM::ISensor^ tmp = get_sensor(h, identifier);
		if (tmp != nullptr) 
			{
			tmp->ValuesTimeWindow = System::TimeSpan::Zero;
			return tmp; 
			}
		}
	for each (auto s in hardware->Sensors)
		{
		if (s->Identifier->ToString()->Equals(identifier)) { return s; }
		}
	return nullptr;
	}

LHM::IHardware^ get_hardware(LHM::IHardware^ hardware, System::String^ identifier)
	{
	for each (auto h in hardware->SubHardware)
		{
		LHM::IHardware^ tmp = get_hardware(h, identifier);
		if (tmp != nullptr) { return tmp; }
		}
	return nullptr;
	}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== SENSOR ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
class SensorDirect::impl
	{
	public:
		gcroot<LHM::ISensor^> inner;
	public:
		impl(LHM::ISensor^ inner) : inner(inner) {}

	};

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== HARDWARE ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 
class Hardware::impl
	{
	public:
		gcroot<LHM::IHardware^> inner;
	public:
		impl(LHM::IHardware^ inner) : inner(inner) {}

	};

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== COMPUTER ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 

class Computer::impl
	{
	public:
		gcroot<LHM::Computer^> inner;
	public:
		impl()
			{
			inner = gcnew LHM::Computer();
			inner->IsControllerEnabled  = true;
			inner->IsCpuEnabled         = true;
			inner->IsGpuEnabled         = true;
			inner->IsMemoryEnabled      = true;
			inner->IsMotherboardEnabled = true;
			inner->IsNetworkEnabled     = false;
			inner->IsStorageEnabled     = false;

			try { inner->Open(); }
			catch (System::Exception^ e)
				{
				System::Console::WriteLine(" ===== EXCEPTION ===== ");
				System::Console::WriteLine(e->ToString());

				System::Console::WriteLine(" ===== ADDITIONAL DATA ===== ");
				for each (auto element in e->Data)
					{
					System::Console::WriteLine(element->ToString());
					}

				System::Console::WriteLine(" ===== STACK TRACE ===== ");
				System::Console::WriteLine(e->StackTrace);
				}
			}

		~impl() { inner->Close(); }
	};


// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== COMPUTER WRAPPER ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 

Computer::Computer()
	:impl_(std::make_unique<impl>())
	{}
Computer::~Computer() = default;

std::unique_ptr<Sensor> Computer::get_sensor(const std::string& identifier) const noexcept
	{
	System::String^ csidentifier = msclr::interop::marshal_as<System::String^>(identifier);
	for each (auto h in impl_->inner->Hardware)
		{
		LHM::ISensor^ tmp = ::get_sensor(h, csidentifier);
		if (tmp != nullptr)
			{
			SensorDirect::impl impl(tmp);
			return std::make_unique<SensorDirect>(impl);
			}
		}
	return {};
	}
std::unique_ptr<Hardware> Computer::get_hardware(const std::string& identifier) const noexcept
	{
	System::String^ csidentifier = msclr::interop::marshal_as<System::String^>(identifier);
	for each (auto h in impl_->inner->Hardware)
		{
		LHM::IHardware^ tmp = ::get_hardware(h, csidentifier);
		if (tmp != nullptr)
			{
			Hardware::impl impl(tmp);
			return std::make_unique<Hardware>(impl);
			}
		}
	return {};
	}


std::string _get_name(LHM::IHardware^ hardware, const std::string& identifier) noexcept
	{
	if (msclr::interop::marshal_as<std::string>(hardware->Identifier->ToString()) == identifier) { return msclr::interop::marshal_as<std::string>(hardware->Name); }

	for each (auto h in hardware->SubHardware)
		{
		auto s = _get_name(h, identifier); if (s != "") { return s; }
		}
	for each (auto s in hardware->Sensors)
		{
		if (msclr::interop::marshal_as<std::string>(s->Identifier->ToString()) == identifier) { return msclr::interop::marshal_as<std::string>(s->Name); }
		}

	return "";
	}

std::string Computer::get_name(const std::string& identifier) const noexcept
	{
	for each (auto h in impl_->inner->Hardware) 
		{
		auto s = _get_name(h, identifier); if (s != "") { return s; }
		}
	return "Name for " + identifier + " not found!";
	}

void Computer::update() noexcept
	{
	for each (auto h in impl_->inner->Hardware) { Hardware::impl impl(h); Hardware(impl).update(); }
	}

std::string Computer::to_string() const noexcept
	{
	std::string ret = "";
	for each (auto h in impl_->inner->Hardware) { Hardware::impl impl(h); ret += Hardware(impl).to_string() += "\n"; }
	return ret;
	}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== SENSOR WRAPPER ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 

SensorDirect::SensorDirect(const SensorDirect::impl& inner) : impl_(std::make_unique<impl>(inner)) {}
SensorDirect::~SensorDirect() = default;

std::string SensorDirect::name() const noexcept { return msclr::interop::marshal_as<std::string>(impl_->inner->Name); }

std::string SensorDirect::identifier() const noexcept { return msclr::interop::marshal_as<std::string>(impl_->inner->Identifier->ToString()); }

sensor_type SensorDirect::type() const noexcept { return sensor_type_conversion(impl_->inner->SensorType); }

std::optional<float> SensorDirect::value() const noexcept
	{
	if (impl_->inner->Value.HasValue) { return std::make_optional<float>(impl_->inner->Value.Value); }
	else { return {}; }
	}

std::string SensorDirect::to_string() const noexcept { return to_string(0); }
std::string SensorDirect::to_string(uint8_t depth) const noexcept
	{
	return indent(depth) + "Sensor: " + name() + " | " + identifier() + " | " + sensor_type_to_string(type()) + "\n";
	}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== HARDWARE WRAPPER ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 

Hardware::Hardware(const Hardware::impl & inner) : impl_(std::make_unique<impl>(inner)) {}
Hardware::~Hardware() = default;

std::string Hardware::name() const noexcept { return msclr::interop::marshal_as<std::string>(impl_->inner->Name); }

std::string Hardware::identifier() const noexcept { return msclr::interop::marshal_as<std::string>(impl_->inner->Identifier->ToString()); }

void Hardware::update() noexcept
	{
	impl_->inner->Update();
	for each (auto h in impl_->inner->SubHardware)  { Hardware::impl impl(h); Hardware(impl).update(); }
	}

std::string Hardware::to_string() const noexcept { return to_string(0); }
std::string Hardware::to_string(uint8_t depth) const noexcept
	{
	std::string ret = indent(depth) + "Hardware: " + name() + " | " + identifier() + "\n";
	for each (auto h in impl_->inner->SubHardware) { Hardware::impl impl(h); ret += Hardware(impl).to_string(depth + 1); }
	for each (auto s in impl_->inner->Sensors) { SensorDirect::impl impl(s); ret += SensorDirect(impl).to_string(depth + 1); }
	return ret;
	}

std::string SensorSum::to_string(uint8_t depth) const noexcept
	{
	return indent(depth) + s1->to_string() + "|" + s2->to_string();
	}

// ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== PRINTER ===== ===== ===== ===== ===== ===== ===== ===== ===== ===== 

void Print()
	{
	LHM::Computer^ computer = gcnew LHM::Computer();
	computer->IsCpuEnabled = true;
	computer->IsGpuEnabled = true;
	computer->IsMemoryEnabled = true;
	computer->IsMotherboardEnabled = true;
	computer->IsControllerEnabled = true;
	computer->IsNetworkEnabled = true;
	computer->IsStorageEnabled = true;

	computer->Open();

	for each(LHM::IHardware^ hardware in computer->Hardware)
		{
		hardware->Update();
		System::Console::WriteLine(hardware->Sensors->Length);
		System::Console::WriteLine("Hardware: {0}", hardware->Name);

		for each(LHM::IHardware^ subhardware in hardware->SubHardware)
			{
			subhardware->Update();
			System::Console::WriteLine("\tSubhardware: {0}", subhardware->Name);
		
			for each(LHM::ISensor^ sensor in subhardware->Sensors)
				{
				System::Console::WriteLine("\t\tSensor: {0}, id: {1}", sensor->Name, sensor->Identifier);
				}
			}

		for each(LHM::ISensor^ sensor in hardware->Sensors)
			{
			System::Console::WriteLine("\tSensor: {0}, id: {1}", sensor->Name, sensor->Identifier);
			}
		}

	computer->Close();
	}