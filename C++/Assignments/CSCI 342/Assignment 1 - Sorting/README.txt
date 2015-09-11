The purpose of this assignment was to compare the speeds of a “stupid sort”, alongside Quick sort, and Merge sort, and report the findings. It was assumed that the speeds of Quick sort, and Merge sort would be many times faster than that of the stupid sort that we selected. In my case, I selected the “stupid sort” known as Bubble sort. 
	
Algorithm	Best-Case	Worst-Case	Average-Case
Bubble Sort	O(n) 		O(n^2)		O(n^2)
Quick Sort	O(n log ?n)	O(n^2)		O(n log? n)
Merge Sort	O(n log? n)	O(n)		O(n log? n)

	The figure above indicates that when performing in the best, and average case Quick sort and Merge sort will be much faster at sorting than Bubble sort. The figure also indicates that in the worst case, Quick sort might be just as bad as Bubble sort. To test the sorts, I implemented a C++ program to generate a data set of 10,000 random integers that would then be sorted by all three sorts 10 times. To ensure that the results were accurate I enforced the sorting of the same 10,000 integers each time for each algorithm. The results are below in terms of seconds:

Bubble sort results: 

Run [0] : 0.298
Run [1] : 0.293
Run [2] : 0.3
Run [3] : 0.299
Run [4] : 0.303
Run [5] : 0.298
Run [6] : 0.296
Run [7] : 0.296
Run [8] : 0.295
Run [9] : 0.301	

Quick sort results: 

Run [0] : 0.004
Run [1] : 0.004
Run [2] : 0.004
Run [3] : 0.004
Run [4] : 0.004
Run [5] : 0.004
Run [6] : 0.004
Run [7] : 0.004
Run [8] : 0.004
Run [9] : 0.004	

Merge sort results: 

Run [0] : 0.001
Run [1] : 0.002
Run [2] : 0.001
Run [3] : 0.002
Run [4] : 0.001
Run [5] : 0.001
Run [6] : 0.001
Run [7] : 0.001
Run [8] : 0.001
Run [9] : 0.001
