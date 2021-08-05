using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Utils
	{
	class ValuesConversions
		{
		public static string load_na() { return "N/A %"; }
		public static string load(float? value) { if (value.HasValue) { return load(value.Value); } return load_na(); ; }
		public static string load(float value)
			{
			var tmp = System.Math.Round(value);
			if (tmp > 100 || tmp < 0) { return load_na(); }
			return string.Format("{0,3:##0}%", tmp);
			}

		public static string temperature_na() { return "N/A °C"; }
		public static string temperature(float? value) { if (value.HasValue) { return temperature(value.Value); } return temperature_na(); }
		public static string temperature(float value)
			{
			var tmp = System.Math.Round(value);
			if (tmp > 999 || tmp < -999) { return temperature_na(); }
			return string.Format("{0,3:##0}°C", tmp);
			}

		public static string fan_na() { return " N/A RPM"; }
		public static string fan(float? value) { if (value.HasValue) { return fan(value.Value); } return fan_na(); }
		public static string fan(float value)
			{
			var tmp = System.Math.Round(value);
			if (tmp > 9999 || tmp < 0) { return fan_na(); }
			return string.Format("{0,4:###0}RPM", tmp);
			}

		//library's ???
		public static string dataTB_na() { return " N/A TB"; }
		public static string dataTB(float? value) { if (value.HasValue) { return dataTB(value.Value); } return dataTB_na(); }
		public static string dataTB(float value)
			{
			if (value >= 1024) { return dataTB_na(); ; }
			if (value < 1) { return dataGB(value * 1024); }
			return string.Format("{0,4:####}TB", value);
			}

		//library's data
		public static string dataGB_na() { return " N/A GB"; }
		public static string dataGB(float? value) { if (value.HasValue) { return dataGB(value.Value); } return dataGB_na(); }
		public static string dataGB(float value)
			{
			if (value >= 1024) { return dataTB(value / 1024); }
			if (value < 1) { return dataMB(value * 1024); }
			return string.Format("{0,4:####}GB", value);
			}

		//library's smalldata
		public static string dataMB_na() { return " N/A MB"; }
		public static string dataMB(float? value) { if (value.HasValue) { return dataMB(value.Value); } return dataMB_na(); }
		public static string dataMB(float value)
			{
			if (value >= 1024) { return dataGB(value / 1024); }
			if (value < 1) { return dataB(value * 1024); }
			return string.Format("{0,4:####}MB", value);
			}

		//library's ???
		public static string dataB_na() { return " N/A B "; }
		public static string dataB(float? value) { if (value.HasValue) { return dataB(value.Value); } return dataTB_na(); }
		public static string dataB(float value)
			{
			if (value >= 1024) { return dataMB(value / 1024); }
			if (value < 1) { return dataB_na(); }
			return string.Format("{0,4:####}B ", value);
			}
		}
	}
