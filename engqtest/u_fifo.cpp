#include <m_eng.h>

// fifo interface
template<typename T>
class IFifo {
protected:
	U32 fifoSize;
public:
	IFifo(U32 sizeA) : fifoSize(sizeA) {}
	virtual bool canWrite(U32 num) const = 0;
	virtual bool canRead(U32 num) const = 0;
	virtual void write(U32 num, const T* dataA) = 0;
	virtual void read(U32 num, T* dataA) = 0;
};

// naive implementation
template<typename T>
class simpleFifo : public IFifo<T> {
	U32 used;
	T* fifoData;
public:
	simpleFifo(U32 sizeA) : IFifo<T>(sizeA), used(0)
	{
		fifoData = new T[sizeA];
	}
	bool canWrite(U32 num) const;
	bool canRead(U32 num) const;
	void write(U32 num, const T* dataA);
	void read(U32 num, T* dataA);
	~simpleFifo()
	{
		delete[] fifoData;
	}
};

template<typename T>
bool simpleFifo<T>::canWrite(U32 num) const
{
	return num <= IFifo<T>::fifoSize - used;
}

template<typename T>
bool simpleFifo<T>::canRead(U32 num) const
{
	return num <= used;
}

template<typename T>
void simpleFifo<T>::write(U32 num, const T* data)
{
	copy(data, data + num, fifoData + used);
	used += num;
}

template<typename T>
void simpleFifo<T>::read(U32 num, T* data)
{
	copy(fifoData, fifoData + num, data);
	copy(fifoData + num, fifoData + used, fifoData);
	used -= num;
}

// better implementation
template<typename T>
class betterFifo : public IFifo<T> {
	U32 used;
	U32 start, end;
	T* fifoData;
public:
	betterFifo(U32 sizeA) : IFifo<T>(sizeA), used(0), start(0), end(0)
	{
		fifoData = new T[sizeA];
	}
	bool canWrite(U32 num) const;
	bool canRead(U32 num) const;
	void write(U32 num, const T* dataA);
	void read(U32 num, T* dataA);
	~betterFifo()
	{
		delete[] fifoData;
	}
};

template<typename T>
bool betterFifo<T>::canWrite(U32 num) const
{
	return num <= IFifo<T>::fifoSize - used;
}

template<typename T>
bool betterFifo<T>::canRead(U32 num) const
{
	return num <= used;
}

template<typename T>
void betterFifo<T>::write(U32 num, const T* data)
{
	if (end + num > IFifo<T>::fifoSize) {
		// wrap
		U32 first = IFifo<T>::fifoSize - end; // amount of first copy
		U32 second = num - first; // amount of second copy
		copy(data, data + first, fifoData + end);
		copy(data + first, data + first + second, fifoData);
		end = second;
		//logger("wrap write %d, %d\n", first, second);
	} else {
		// no wrap
		copy(data, data + num, fifoData + end);
		end += num;
		if (end >= IFifo<T>::fifoSize) {
			end -= IFifo<T>::fifoSize;
		}
	}
	used += num;
}

template<typename T>
void betterFifo<T>::read(U32 num, T* data)
{
	if (start + num > IFifo<T>::fifoSize) {
		// wrap
		U32 first = IFifo<T>::fifoSize - start; // amount of first copy
		U32 second = num - first; // amount of second copy
		copy(fifoData + start, fifoData + start + first, data);
		copy(fifoData, fifoData + second, data + first);
		start = second;
		//logger("wrap read %d, %d\n", first, second);
	} else {
		// no wrap
		copy(fifoData + start, fifoData + start + num, data);
		start += num;
		if (start >= IFifo<T>::fifoSize) {
			start -= IFifo<T>::fifoSize;
		}
	}
	used -= num;
}

void do_fifo()
{
	logger("<<<<<<<<<< in do_fifo >>>>>>>>>>\n");
	const U32 mt_seed = 3456;
	const U32 num_tests = 1000;
	const U32 fifo_size = 10;
	mt_setseed(mt_seed);
	const U32 min_rw = 1;
	const U32 max_rw = fifo_size + 2;
	float writeArr[max_rw];
	float readArr0[max_rw];
	float readArr1[max_rw];
	simpleFifo<float> fifo0(fifo_size);
	betterFifo<float> fifo1(fifo_size);
	float values = 100.0f;
	float checkValue = values;
	const float incValue = 10.0f;
	for (U32 i = 0; i < num_tests; ++i) {
		U32 rw_count = mt_random(max_rw - min_rw + 1) + min_rw;
		if (mt_random(2)) {
			// write
			bool cw0 = fifo0.canWrite(rw_count);
			bool cw1 = fifo1.canWrite(rw_count);
			if (cw0 != cw1) {
				errorexit("canWrite: inconsistent");
			}
			if (cw0) {
				for (U32 j = 0; j < rw_count; ++j) {
					writeArr[j] = values;
					values += incValue;
				}
				fifo0.write(rw_count, writeArr);
				fifo1.write(rw_count, writeArr);
			}
		} else {
			// read
			bool cr0 = fifo0.canRead(rw_count);
			bool cr1 = fifo1.canRead(rw_count);
			if (cr0 != cr1) {
				errorexit("canRead: inconsistent");
			}
			if (cr0) {
				fifo0.read(rw_count, readArr0);
				fifo1.read(rw_count, readArr1);
				if (!equal(readArr0, readArr0 + rw_count, readArr1)) {
					errorexit("fifo read values are inconsistent");
				} else {
					for (U32 j = 0; j < rw_count; ++j) {
						float cv = readArr0[j];
						if (cv != checkValue) {
							errorexit("bad check value");
						}
						checkValue += incValue;
						//logger("val = %f\n", cv);
					}
				}
			}
		}
	}
}
