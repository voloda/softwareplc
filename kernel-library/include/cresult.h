/*
 *  This file is part of Software PLC for RT-Linux.
 *
 *  Software PLC for RT-Linux is free software; you can redistribute 
 *  it and/or modify it under the terms of the GNU General Public 
 *  License as published by the Free Software Foundation; either 
 *  version 2 of the License, or (at your option) any later version.
 *
 *  Software PLC for RT-Linux is distributed in the hope that it 
 *  will be useful, but WITHOUT ANY WARRANTY; without even the implied 
 *  warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Software PLC for RT-Linux; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Author: Vladimir Kloz <Vladimir.Kloz@dtg.cz>
 *  Project home: http://sourceforge.net/projects/softwareplc
 *  Version: $Revision: 1.11 $
 */

#ifndef _RTL_PLC_CRESULT
	#define _RTL_PLC_RESULT

	#include "data_types.h"

	#include "cvariable.h"
	#include "cpropertyin.h"
	#include "cpropertyio.h"

	/**
	 * Enumaration used for current data type
	 * value stored in CResult class
	 */
	typedef enum {
		DT_Empty,
		DT_SINT,
		DT_INT,
		DT_DINT,
		DT_USINT,
		DT_UINT,
		DT_UDINT,
		DT_REAL,
		DT_TIME,
		DT_DATE,
		DT_TOD,
		DT_DT,
		DT_BYTE,
		DT_WORD,
		DT_DWORD
	} EnumStoredDT;

	/**
	 * Macro for short implementation of constructor for
	 * specified data type
	 *
	 * @param p	PLC data type of declared constructor
	 */
	#define CResultC(p)	CResult(const p Value) { m_iStoredDataType = DT_##p; InternalData.m_##p##_Value = Value;}

	/**
	 * Macro for short implementation of arithmetic and logical operators for
	 * specified data type
	 *
	 * @param dt	Specified data type
	 * @param op	Implemented operator (+,-,/ ....)
	 */
	#define CResultOp(dt, op)	case DT_##dt: this->InternalData.m_##dt##_Value op##= Value.Result##dt(); break;

	/**
	 * Macro for short implementation of comparing operators
	 * for specified data type
	 *
	 * @param dt	Specified data type
	 * @param op	Implemented operator (<,<=,==, ...)
	 */
	#define CResultCmpOp(dt, op)	case DT_##dt: return(InternalData.m_##dt##_Value op Value.Result##dt()); break;

	/**
	 * Macro for short implementation when getting
	 * current result into CVariable<> or CProperty<>
	 * classes
	 *
	 * @param dt	Internaly stored data type
	 */
	#define CResultGV(dt)	case DT_##dt: Var =  (DataType) InternalData.m_##dt##_Value; break;

	/**
	 * Macro for short implementation when getting
	 * negated current result into CVariable<> or CProperty<>
	 * classes
	 *
	 * @param dt	Internaly stored data type
	 */
	#define CResultNegGV(dt)	case DT_##dt: Var =  ~((DataType) InternalData.m_##dt##_Value); break;

	/**
	 * Macro for short implementation when getting internaly
	 * stored value in operators
	 *
	 * @param idt	Internal data type stored in result
	 * @param rdt	Requested data type into which convert result
	 */
	#define CResultDT(idt, rdt)	case DT_##idt: return ((rdt) (this->InternalData.m_##idt##_Value)); break;
	/**
	 * Macro for short implementation when getting current
	 * value of result converted to specified datatype
	 *
	 * @param dt	Requested datatype
	 */
	#define ImplementResult(dt)		dt Result##dt(void) const \
					{\
						switch(m_iStoredDataType) \
						{\
							CResultDT(SINT, dt);\
							CResultDT(INT, dt);\
							CResultDT(DINT, dt);\
							CResultDT(USINT, dt);\
							CResultDT(UINT, dt);\
							CResultDT(UDINT, dt);\
							CResultDT(REAL, dt);\
							CResultDT(TIME, dt);\
							CResultDT(BYTE, dt);\
							CResultDT(WORD, dt);\
							CResultDT(DWORD, dt);\
						}\
						return ((dt)0);\
					}
	/**
	 * This class implements  result functionality for PLC
	 * IL language operations.
	 * All operations (comparisions, additions, multiplications...)
	 * are defined here as operators
	 *
	 * @version	$Revision: 1.11 $
	 * @author	Voloda <Vladimir.Kloz@dtg.cz>
	 */
	class CResult
	{
		private:
			/**
			 * Union defining space for storage of current
			 * result. Information about current result
			 * data type is stored in m_iStoredDataType
			 * instance variable
			 */
			union 
			{
				SINT	m_SINT_Value;
				INT	m_INT_Value;
				DINT	m_DINT_Value;
				USINT	m_USINT_Value;
				UINT	m_UINT_Value;
				UDINT	m_UDINT_Value;
				REAL	m_REAL_Value;
				TIME	m_TIME_Value;
				/*
				 * Not supported yet
				DATE	m_dValue;
				TOD	m_todValue;
				DT	m_dtValue;
				*/
				BYTE	m_BYTE_Value;
				WORD	m_WORD_Value;
				DWORD	m_DWORD_Value;
			} InternalData;
			
			/*
			 * Implementation of functions, which returns
			 * internal value converted into specified datatype
			 */
			ImplementResult(SINT);
			ImplementResult(INT);
			ImplementResult(DINT);
			ImplementResult(USINT);
			ImplementResult(UINT);
			ImplementResult(UDINT);
			ImplementResult(REAL);
			ImplementResult(TIME);
			ImplementResult(BYTE);
			ImplementResult(WORD);
			ImplementResult(DWORD);

			/**
			 * Contain information about current data type
			 * stored in result object as defined in
			 * enumeration EnumStoredDT
			 */
			EnumStoredDT	m_iStoredDataType;
		public:
			/**
			 * Default class constructor
			 */
			CResult(void)
			{
				m_iStoredDataType = DT_Empty;
			};
			
			/**
			 * Implemented class constructors for standard
			 * data types
			 */
			CResultC(SINT);
			CResultC(INT);
			CResultC(DINT);
			CResultC(USINT);
			CResultC(UINT);
			CResultC(UDINT);
			CResultC(REAL);

			/**
			 * Class constructor for initialization with CVariable
			 * class
			 *
			 * @param vValue	Given CVariable variable class from which initialize
			 */
			template<class VariableType> CResult(const CVariable<VariableType>& vValue)
			{
				VariableType	tmp;
				CResult		result;

				tmp = CVariable<VariableType>(vValue);
				
				result = (VariableType) tmp;

				m_iStoredDataType = result.m_iStoredDataType;

				InternalData = result.InternalData;
				// memcpy((void *) &InternalData, (void *) &(result.InternalData), sizeof(InternalData));
			};
			
			/**
			 * Class constructor for initialization with CProperty class
			 *
			 * @param pValue	Given CProperty variable class from which initialize
			 */
			template<class PropertyType> CResult(const CPropertyOut<PropertyType>& pValue)
			{
				PropertyType	tmp;
				CResult		result;

				tmp = CPropertyOut<PropertyType>(pValue);
				
				result = (PropertyType) tmp;

				m_iStoredDataType = result.m_iStoredDataType;

				InternalData = result.InternalData;

				// memcpy((void *) &InternalData, (void *) &(result.InternalData), sizeof(InternalData));
			
			};

			/**
			 * Class constructor for initialization with CProperty class
			 *
			 * @param pValue	Given CProperty variable class from which initialize
			 */
			template<class PropertyType> CResult(const CPropertyIO<PropertyType>& pValue)
			{
				PropertyType	tmp;
				CResult		result;

				tmp = CPropertyIO<PropertyType>(pValue);
				
				result = (PropertyType) tmp;

				m_iStoredDataType = result.m_iStoredDataType;

				InternalData = result.InternalData;

				// memcpy((void *) &InternalData, (void *) &(result.InternalData), sizeof(InternalData));
			
			};
			/**
			 * Implementation of standard arithmetic operators
			 */
			CResult& operator +(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultOp(SINT, +);
					CResultOp(INT, +);
					CResultOp(DINT, +);
					CResultOp(USINT, +);
					CResultOp(UINT, +);
					CResultOp(UDINT, +);
					CResultOp(REAL, +);
				}

				return(*this);
			};
			CResult& operator -(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultOp(SINT, -);
					CResultOp(INT, -);
					CResultOp(DINT, -);
					CResultOp(USINT, -);
					CResultOp(UINT, -);
					CResultOp(UDINT, -);
					CResultOp(REAL, -);
				}

				return(*this);
			};

			CResult& operator *(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultOp(SINT, *);
					CResultOp(INT, *);
					CResultOp(DINT, *);
					CResultOp(USINT, *);
					CResultOp(UINT, *);
					CResultOp(UDINT, *);
					CResultOp(REAL, *);
				}

				return(*this);
			};

			CResult& operator /(const CResult& Value)
			{
				/* Check against division by zero in
				 * kernel space. Returns current value
				 * stored in result
				 */
				if (Value.ResultINT() == 0)
				{
					rtl_printf("SoftwarePLC: Division by zerp detected, returned result is wrong!");
					return (*this);
				}

				switch(m_iStoredDataType)
				{
					CResultOp(SINT, /);
					CResultOp(INT, /);
					CResultOp(DINT, /);
					CResultOp(USINT, /);
					CResultOp(UINT, /);
					CResultOp(UDINT, /);
					CResultOp(REAL, /);
				}

				return(*this);
			};
		
			CResult& operator |(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultOp(SINT, |);
					CResultOp(INT, |);
					CResultOp(DINT, |);
					CResultOp(USINT, |);
					CResultOp(UINT, |);
					CResultOp(UDINT, |);
				}

				return(*this);
			};

			CResult& operator &(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultOp(SINT, &);
					CResultOp(INT, &);
					CResultOp(DINT, &);
					CResultOp(USINT, &);
					CResultOp(UINT, &);
					CResultOp(UDINT, &);
				}

				return(*this);
			};

			CResult& operator ^(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultOp(SINT, ^);
					CResultOp(INT, ^);
					CResultOp(DINT, ^);
					CResultOp(USINT, ^);
					CResultOp(UINT, ^);
					CResultOp(UDINT, ^);
				}

				return(*this);
			};

			/**
			 * Implementation of standard comparision operators
			 */
			bool operator <=(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultCmpOp(SINT, <=);
					CResultCmpOp(INT, <=);
					CResultCmpOp(DINT, <=);
					CResultCmpOp(USINT, <=);
					CResultCmpOp(UINT, <=);
					CResultCmpOp(UDINT, <=);
					CResultCmpOp(REAL, <=);

					default:
						break;
				}

				return(false);
			};

			bool operator <(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultCmpOp(SINT, <);
					CResultCmpOp(INT, <);
					CResultCmpOp(DINT, <);
					CResultCmpOp(USINT, <);
					CResultCmpOp(UINT, <);
					CResultCmpOp(UDINT, <);
					CResultCmpOp(REAL, <);

					default:
						break;
				}

				return(false);
			};

			bool operator ==(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultCmpOp(SINT, ==);
					CResultCmpOp(INT, ==);
					CResultCmpOp(DINT, ==);
					CResultCmpOp(USINT, ==);
					CResultCmpOp(UINT, ==);
					CResultCmpOp(UDINT, ==);
					CResultCmpOp(REAL, ==);

					default:
						break;
				}

				return(false);
			};

			bool operator !=(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultCmpOp(SINT, !=);
					CResultCmpOp(INT, !=);
					CResultCmpOp(DINT, !=);
					CResultCmpOp(USINT, !=);
					CResultCmpOp(UINT, !=);
					CResultCmpOp(UDINT, !=);
					CResultCmpOp(REAL, !=);

					default:
						break;
				}

				return(false);
			};

			bool operator >=(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultCmpOp(SINT, >=);
					CResultCmpOp(INT, >=);
					CResultCmpOp(DINT, >=);
					CResultCmpOp(USINT, >=);
					CResultCmpOp(UINT, >=);
					CResultCmpOp(UDINT, >=);
					CResultCmpOp(REAL, >=);

					default:
						break;
				}

				return(false);
			};

			bool operator >(const CResult& Value)
			{
				switch(m_iStoredDataType)
				{
					CResultCmpOp(SINT, >);
					CResultCmpOp(INT, >);
					CResultCmpOp(DINT, >);
					CResultCmpOp(USINT, >);
					CResultCmpOp(UINT, >);
					CResultCmpOp(UDINT, >);
					CResultCmpOp(REAL, >);

					default:
						break;
				}

				return(false);
			};


			CResult& operator=(const CResult& Value)
			{
				if (this != &Value)
				{
					m_iStoredDataType = Value.m_iStoredDataType;
					InternalData = Value.InternalData;
				}

				return(*this);
			};

			/**
			 * Function for getting current result value
			 * and store it into CProperty<> class
			 *
			 * @param	Var	Variable of type CProperty<> into which store value
			 */
			template<class DataType> void GetValue(CPropertyIn<DataType>& Var)
			{
				switch(m_iStoredDataType)
				{
					CResultGV(SINT);
					CResultGV(INT);
					CResultGV(DINT);
					CResultGV(USINT);
					CResultGV(UINT);
					CResultGV(UDINT);
					CResultGV(REAL);

					default:
						break;
				}
			};

			/**
			 * Function for getting current result value
			 * and store it into CProperty<> class
			 *
			 * @param	Var	Variable of type CProperty<> into which store value
			 */
			template<class DataType> void GetValue(CPropertyIO<DataType>& Var)
			{
				switch(m_iStoredDataType)
				{
					CResultGV(SINT);
					CResultGV(INT);
					CResultGV(DINT);
					CResultGV(USINT);
					CResultGV(UINT);
					CResultGV(UDINT);
					CResultGV(REAL);

					default:
						break;
				}
			};
		
			/**
			 * Function for getting current result value
			 * and store it into CVariable<> class
			 *
			 * @param	Var	Variable of type CVariable<> into which store value
			 */
			template<class DataType> void GetValue(CVariable<DataType>& Var)
			{
				switch(m_iStoredDataType)
				{
					CResultGV(SINT);
					CResultGV(INT);
					CResultGV(DINT);
					CResultGV(USINT);
					CResultGV(UINT);
					CResultGV(UDINT);
					CResultGV(REAL);

					default:
						break;
				}
			};

			template<class DataType> void GetValueNeg(CVariable<DataType>& Var)
			{
				switch(m_iStoredDataType)
				{
					CResultNegGV(SINT);
					CResultNegGV(INT);
					CResultNegGV(DINT);
					CResultNegGV(USINT);
					CResultNegGV(UINT);
					CResultNegGV(UDINT);
					CResultNegGV(REAL);

					default:
						break;
				}
			}

	};
#endif

