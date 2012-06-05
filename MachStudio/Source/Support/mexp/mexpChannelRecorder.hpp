/****************************************************************************\
**	mexpChannelRecorder.hpp
**
**		mexpChannelRecorder attaches to timeline channels and properties
**	in order to convert simulated data into a set of key frames.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MEXP_CHANNELRECORDER_HPP
#error mexpChannelRecorder.hpp multiply included
#endif
#define MEXP_CHANNELRECORDER_HPP

#include <map>

//============================================================================
//	Forward References
//============================================================================
class mexpExporter;

//============================================================================
// Base class for recording (baking) the animation of a channel for export
//============================================================================
class mexpChannelRecorder
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~mexpChannelRecorder() { }

	//--------------------------------------------------------------------
	//	Record frame of data at given time
	//--------------------------------------------------------------------
	virtual void RecordFrame(float i_CurrentTime) = 0;

	//--------------------------------------------------------------------
	//	End of recording, add delayed data to keys
	//--------------------------------------------------------------------
	virtual void FinishRecording() = 0;

	//--------------------------------------------------------------------
	//	Export data to the exporter
	//--------------------------------------------------------------------
	virtual void Export(mexpExporter &io_Exporter) = 0;

};

//============================================================================
// Utility class for recording keys and removing some that are not 
// necessary in order to reduce file size.
//============================================================================
template<class T>
class mexpRecorderTemplate
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	mexpRecorderTemplate()
		: m_bDelay(false)
	{

	}

	//--------------------------------------------------------------------
	//	Consider a value at a given time, will check to see if
	//	the key could have been interpolated by other frames.
	//--------------------------------------------------------------------
	void SubmitValue(float i_CurrentTime, const T& i_Value)
	{
		// Simpler implementation is this line, but writes out
		// a value every frame.
		//m_Keys[i_CurrentTime] = i_Value;

		// More complicated implementation...
		// Compare val to previous frame, try not to 
		// add three of the same frame values in a row.
		//
		if ((!m_Keys.empty()) &&
			(i_Value == m_LastValue) )
		{
			// delay this frame
			m_bDelay = true;
			m_DelayedTime = i_CurrentTime;
		}
		else
		{
			if (m_bDelay)
			{
				// if delaying, add delayed frame now
				m_Keys[m_DelayedTime] = m_LastValue;
				m_bDelay = false;
			}

			// add current key value
			m_Keys[i_CurrentTime] = i_Value;
			m_LastValue = i_Value;
		}
	}

	//--------------------------------------------------------------------
	//	End of recording, add delayed data to keys
	//--------------------------------------------------------------------
	void FinishRecording()
	{
		// Hit end of list. If still delaying frame, write it now
		if (m_bDelay)
		{
			m_Keys[m_DelayedTime] = m_LastValue;
			m_bDelay = false;
		}
	}


	std::map<float, T> m_Keys;

	// Delay the actual storing of the current key until it is known
	// that this key info is important (cannot be interpolated from other keys)
	bool	m_bDelay;
	T		m_LastValue;
	float	m_DelayedTime;
};

//============================================================================
// Derived, but still abstract recorder  class that can be attached to a 
// property in order to record its changes over time
//============================================================================
template<class T, class xxxProperty>
class mexpRecorderPropertyTemplate : public mexpChannelRecorder
{
public:
	//--------------------------------------------------------------------
	//	Record frame of data at given time
	//--------------------------------------------------------------------
	virtual void RecordFrame(float i_CurrentTime)
	{
		m_Recorder.SubmitValue(i_CurrentTime, m_Property.GetValue());
	}

	//--------------------------------------------------------------------
	//	End of recording, add delayed data to keys
	//--------------------------------------------------------------------
	virtual void FinishRecording()
	{
		m_Recorder.FinishRecording();
	}

protected:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	explicit mexpRecorderPropertyTemplate(const xxxProperty &i_Property)
		: m_Property(i_Property)
	{

	}

	const xxxProperty &m_Property;
	mexpRecorderTemplate<T> m_Recorder;
};
