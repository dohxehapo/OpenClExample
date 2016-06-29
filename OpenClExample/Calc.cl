__kernel void Calculate(
	__global const int *pSrc1,
	__global const int *pSrc2,
	__global int *pDest,
	int pTotalSize)
{
	// Get global index
	int index = get_global_id(0);

	// Check boundaries
	if (index >= pTotalSize) return;

	*(pDest + index) = *(pSrc1 + index) + *(pSrc2 + index);
	if (*(pDest + index) >= 256)
		*(pDest + index) -= 256;
}
