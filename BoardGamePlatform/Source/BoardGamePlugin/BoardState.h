#ifndef BG_BOARD_STATE_H__
#define BG_BOARD_STATE_H__

template <class T>
class BG_BoardState
{
public:
	static T* GetInstance();

private:
	BG_BoardState();

private:
	static T* m_instance;
};

template<class T>
T* BG_BoardState<T>::m_instance = NULL;

template<class T>
BG_BoardState<T>::BG_BoardState()
{
	m_instance = new T();
}

template<class T>
T* BG_BoardState<T>::GetInstance()
{
	if(m_instance == NULL)
	{
		BG_BoardState<T>();
	}

	return m_instance;
}
#endif

