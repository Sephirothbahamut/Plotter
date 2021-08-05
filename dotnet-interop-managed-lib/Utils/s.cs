using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Utils
	{
	class s
		{
		public static int intend_size = 4;
		public static string indent(int amount)
			{
			StringBuilder str = new StringBuilder();
			for (int i = 0; i < intend_size * amount; i++) { str.Append(" "); }
			return str.ToString();
			}
		public static string uniform_length(string str, int length)
			{
			if (str.Length < length) { while (str.Length < length) { str += " "; } return str; }
			else { return str; };
			}
		}
	}
