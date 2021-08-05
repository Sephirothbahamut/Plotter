using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Utils
	{
	public class IndexedProperty<TIndex, TValue>
		{
		readonly Action<TIndex, TValue> SetAction;
		readonly Func<TIndex, TValue> GetFunc;

		public IndexedProperty(Func<TIndex, TValue> getFunc, Action<TIndex, TValue> setAction)
			{
			this.GetFunc = getFunc;
			this.SetAction = setAction;
			}

		public TValue this[TIndex i]
			{
			get
				{
				return GetFunc(i);
				}
			set
				{
				SetAction(i, value);
				}
			}
		}
	public class IndexedPropertyReadOnly<TIndex, TValue>
		{
		readonly Func<TIndex, TValue> GetFunc;

		public IndexedPropertyReadOnly(Func<TIndex, TValue> getFunc)
			{
			this.GetFunc = getFunc;
			}

		public TValue this[TIndex i]
			{
			get
				{
				return GetFunc(i);
				}
			}
		}
	public class IndexedPropertyWriteOnly<TIndex, TValue>
		{
		readonly Action<TIndex, TValue> SetAction;

		public IndexedPropertyWriteOnly(Action<TIndex, TValue> setAction)
			{
			this.SetAction = setAction;
			}

		public TValue this[TIndex i]
			{
			set
				{
				SetAction(i, value);
				}
			}
		}
	/*EXAMPLE
	public class ExampleCollection<T> : Collection<T>
		{
		public IndexedProperty<int, T> ExampleProperty
			{
			get
				{
				return new IndexedProperty<int, T>(GetIndex, SetIndex);
				}
			}

		private T GetIndex(int index)
			{
			return this[index];
			}
		private void SetIndex(int index, T value)
			{
			this[index] = value;
			}
		}*/
	}
