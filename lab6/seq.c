/* File: seq.c
* Author: Ian Myers
* Purpose: sorts an array using quicksort
* Notes:
* 1. This file is a helper function for sort.c
*/
void quick_sort(int *array, int lo, int hi, int flag)
{
    int i = lo-1;
    int j = hi;
    int pivot= array[hi];
    int temp;
    if (hi>lo)
    {
        do
        {
            if(flag == 1) /* a increase sort */
            {
                do i++; while (array[i]<pivot);
                do j--; while (array[j]>pivot);
            }
            else /* a decrease sort */
            {
                do i++; while (array[i]>pivot);
                do j--; while (array[j]<pivot);
            }
            temp = array[i]; /* swap values */
            array[i] = array[j];
            array[j] = temp;
        } while (j>i);
        array[j] = array[i]; /* swap values */
        array[i] = pivot;
        array[hi] = temp;
        quick_sort(array,lo,i-1,flag); /* recursive until hi == lo */
        quick_sort(array,i+1,hi,flag);
    }
}