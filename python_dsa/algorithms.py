#!/usr/bin/env python3
"""
Advanced Algorithms Implementation
Author: jk1806
Created: 2024
"""

import numpy as np
import time

def quick_sort(arr):
    """Quick sort algorithm implementation"""
    if len(arr) <= 1:
        return arr
    
    pivot = arr[len(arr) // 2]
    left = [x for x in arr if x < pivot]
    middle = [x for x in arr if x == pivot]
    right = [x for x in arr if x > pivot]
    
    return quick_sort(left) + middle + quick_sort(right)

def binary_search(arr, target):
    """Binary search algorithm implementation"""
    left, right = 0, len(arr) - 1
    
    while left <= right:
        mid = (left + right) // 2
        if arr[mid] == target:
            return mid
        elif arr[mid] < target:
            left = mid + 1
        else:
            right = mid - 1
    
    return -1

if __name__ == "__main__":
    # Test algorithms
    test_array = [64, 34, 25, 12, 22, 11, 90]
    sorted_array = quick_sort(test_array)
    print(f"Sorted array: {sorted_array}")
    
    # Test binary search
    index = binary_search(sorted_array, 25)
    print(f"Found 25 at index: {index}")
