/*
    File: bitmap.H

    Author: Ningyuan Wang
            Department of Electrical and Computer Engineering
            Texas A&M University
    Date  : 12/01/2014

    Description:


*/

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/
template<unsigned int SIZE>
class BitMap
{
private:
   char array[SIZE];
public:
    BitMap();
    unsigned long get_free(unsigned int length);
    void allocate(unsigned int index);
    void release(unsigned int index);
};


template<unsigned int SIZE>
BitMap<SIZE>::BitMap()
{
	for ( unsigned int index = 0; index < SIZE; index++)
	{
		unsigned int start = index >> 3;
		int offset_index = index & 0x7;
		char mask = 1 << offset_index;
		mask = ~mask;
		array[index >> 3] = array[index >> 3] & mask;
	}
}
template<unsigned int SIZE>
unsigned long BitMap<SIZE>::get_free(unsigned int length)
{
	for( unsigned int index = 0; index < (SIZE - length + 1); index++)
	{
		bool find = true;
		for ( unsigned offset = 0; offset < length; offset++ )
		{
			int offset_index =( index + offset ) & 0x7;
			char mask = 1 << offset_index;
			if( ( mask & array[ ( index + offset) >> 3]  ) != 0 )
				find = false;
		}
		if (find) return index;
	}
	return -1;
}

template<unsigned int SIZE>
void BitMap<SIZE>::allocate(unsigned int index)
{
	unsigned int start = index >> 3;
	int offset_index = index & 0x7;
	char mask = 1 << offset_index;
	array[start] = array[start] | mask;
	return;
}

template<unsigned int SIZE>
void BitMap<SIZE>::release(unsigned int index)
{
        // set the frame bit free
	int offset_index = index  & 0x7;
	char mask = 1 << offset_index;
	mask = ~mask;
	array[index >> 3] = array[index >> 3] & mask;
	return;
}

