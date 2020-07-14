#include <m_eng.h>

template<typename T, typename U>
S32 myBubbleSort(T* begin, T* end, U lt)
{
	S32 swapCount = 0;
	T* base = begin;
	S32 size = end - begin;
	S32 cur = 0;
	while (true) {
		S32 next = cur + 1;
		if (next >= size) {
			return swapCount;
		}
		if (!lt(base[next], base[cur])) {
			++cur;
			continue;
		}
		S32 back = cur;
		swap(base[cur], base[next]);
		++swapCount;
		while (true) {
			--back;
			if (back < 0)
				break;
			if (lt(base[back + 1], base[back])) {
				swap(base[back], base[back + 1]);
				++swapCount;
			} else {
				break;
			}
		}
	}
	return swapCount;
}

// the bubble sort, default 3rd arg didn't seem to work
template<typename T>
S32 myBubbleSort(T* begin, T* end)
{
	return myBubbleSort(begin, end, std::less<T>());
}

inline S32 heapDownLeft(S32 parent)
{
	return (parent << 1) + 1;
}

inline S32 heapUp(S32 child)
{
	return (child - 1) >> 1;
}

template<typename T, typename U>
S32 myHeapSort(T* begin, T* end, U lt)
{
	S32 swapCount = 0;
	T* base = begin;
	S32 size = end - begin;
	if (size < 2)
		return 0;
	// build up the heap, first one is a freebie
	S32 heapSize = 1;
	while (heapSize < size) {
		// validate heap, start at bottom
		S32 cur = heapSize;
		while (cur > 0) {
			S32 next = heapUp(cur);
			if (lt(base[next], base[cur])) {
				swap(base[next], base[cur]);
				++swapCount;
			}
			cur = next;
		}
		++heapSize;
	}
	// tear down the heap
	while (heapSize > 1) {
		--heapSize;
		// heap replace
		swap(base[0], base[heapSize]);
		++swapCount;
		// validate, start at top
		S32 cur = 0;
		while (true) {
			S32 downLeft = heapDownLeft(cur);
			if (downLeft >= heapSize)
				break;
			S32 downRight = downLeft + 1;
			S32 down;
			if (downRight == heapSize) {
				// just the left side
				down = downLeft;
			} else {
				// both sides, favor left side
				down = lt(base[downLeft], base[downRight]) ? downRight : downLeft;
			}
			if (lt(base[cur], base[down])) {
				swap(base[cur], base[down]);
				++swapCount;
				cur = down;
			} else {
				break;
			}
		}
	}
	return swapCount;
}

// the heap sort, default 3rd arg didn't seem to work
template<typename T>
S32 myHeapSort(T* begin, T* end)
{
	return myHeapSort(begin, end, std::less<T>());
}


// comparators
bool myCompGreater(S32 i, S32 j)
{
	return (i > j);
}

bool myEvenOdds(S32 i, S32 j)
{
	return (i & 1) < (j & 1);
}

bool myOddEvens(S32 i, S32 j)
{
	return (i & 1) > (j & 1);
}

bool noCompare(S32 i, S32 j)
{
	return false;
}

void do_printSortArray(S32* arr, U32 numArr)
{
	for (U32 i = 0; i < numArr; ++i) {
		logger("%d", arr[i]);
	}
	logger("\n");
}

void do_heapsort()
{
	logger("========\n");
	logger("in do_heapsort\n");
	const U32 arrSize = 10;
	S32 arr[arrSize]{ 3,9,7,2,4,6,8,1,0,5 };
	logger("original array\n");
	do_printSortArray(arr, arrSize);
	S32 workArray[arrSize];
	logger("sorted by: myBubbleSort, myHeapSort, std::sort\n");
	S32 swapCount;

	// default compare
	logger("\ndefault compare\n");
	copy(arr, arr + arrSize, workArray);
	swapCount = myBubbleSort(workArray, workArray + arrSize);
	logger("bubble swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	swapCount = myHeapSort(workArray, workArray + arrSize);
	logger("heap swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	sort(workArray, workArray + arrSize);
	logger("std::sort\n");
	do_printSortArray(workArray, arrSize);
#if 1

	// reverse compare
	logger("\nreverse compare\n");
	copy(arr, arr + arrSize, workArray);
	swapCount = myBubbleSort(workArray, workArray + arrSize, myCompGreater);
	logger("bubble swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	swapCount = myHeapSort(workArray, workArray + arrSize, myCompGreater);
	logger("heap swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	sort(workArray, workArray + arrSize, myCompGreater);
	logger("std::sort\n");
	do_printSortArray(workArray, arrSize);

	// std less compare
	logger("\nstd less compare\n");
	copy(arr, arr + arrSize, workArray);
	swapCount = myBubbleSort(workArray, workArray + arrSize, std::less<S32>());
	logger("bubble swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	swapCount = myHeapSort(workArray, workArray + arrSize, std::less<S32>());
	logger("heap swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	sort(workArray, workArray + arrSize, std::less<S32>());
	logger("std::sort\n");
	do_printSortArray(workArray, arrSize);

	// even odds
	logger("\neven odds\n");
	copy(arr, arr + arrSize, workArray);
	swapCount = myBubbleSort(workArray, workArray + arrSize, myEvenOdds);
	logger("bubble swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	swapCount = myHeapSort(workArray, workArray + arrSize, myEvenOdds);
	logger("heap swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	sort(workArray, workArray + arrSize, myEvenOdds);
	logger("std::sort\n");
	do_printSortArray(workArray, arrSize);

	// odd evens
	logger("\nodd evens\n");
	copy(arr, arr + arrSize, workArray);
	swapCount = myBubbleSort(workArray, workArray + arrSize, myOddEvens);
	logger("bubble swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	swapCount = myHeapSort(workArray, workArray + arrSize, myOddEvens);
	logger("heap swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	sort(workArray, workArray + arrSize, myOddEvens);
	logger("std::sort\n");
	do_printSortArray(workArray, arrSize);

	// null compare
	logger("\nnull compare\n");
	copy(arr, arr + arrSize, workArray);
	swapCount = myBubbleSort(workArray, workArray + arrSize, noCompare);
	logger("bubble swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	swapCount = myHeapSort(workArray, workArray + arrSize, noCompare);
	logger("heap swapcount = %d\n", swapCount);
	do_printSortArray(workArray, arrSize);

	copy(arr, arr + arrSize, workArray);
	sort(workArray, workArray + arrSize, noCompare);
	logger("std::sort\n");
	do_printSortArray(workArray, arrSize);
#endif

	// try sorting a random float array
	const U32 floatArrSize = 10000;
	float floatArr[floatArrSize];
	float floatWorkArrB[floatArrSize]; // bubble
	float floatWorkArrH[floatArrSize]; // heap
	float floatWorkArrS[floatArrSize]; // std
	//float v = 10;
	for (auto& f : floatArr) {
		f = mt_frand();
		//f = v;
		//v += 10.0f;
		//v -= 10.0f;
	}
	copy(floatArr, floatArr + floatArrSize, floatWorkArrB);
	copy(floatArr, floatArr + floatArrSize, floatWorkArrH);
	copy(floatArr, floatArr + floatArrSize, floatWorkArrS);
	S32 swapCountfB = myBubbleSort(floatWorkArrB, floatWorkArrB + floatArrSize);
	S32 swapCountfH = myHeapSort(floatWorkArrH, floatWorkArrH + floatArrSize);
	sort(floatWorkArrS, floatWorkArrS + floatArrSize);
	if (!equal(floatWorkArrB, floatWorkArrB + floatArrSize, floatWorkArrH)) {
		errorexit("sort between bubble and heap failed");
	}
	if (!equal(floatWorkArrB, floatWorkArrB + floatArrSize, floatWorkArrS)) {
		errorexit("sort between bubble and std::sort failed");
	}
	logger("random float array: number of swaps: bubble %d, heap %d\n", swapCountfB, swapCountfH);
}
