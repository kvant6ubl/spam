#include "auxiliary.h"
#include <iostream>
#include <fstream>
#include "tspamex.h"
#include <vector>

using namespace boost;
using namespace std;

/**
 * @brief Create random walk (permutation). The random number generator is not initialized, do that before.
 *    @param length - length of the random walk
 */
shared_array<int> getRandomWalk(int length)
{
	shared_array<int> walk(new int[length]);
	for (int i = 0; i < length; i++)
	{
		walk[i] = i;
	}

	for (int i = 0; i < length - 1; i++)
	{
		/* Random remaining position */
		int r = i + (rand() % (length - i));
		int temp = walk[i];
		walk[i] = walk[r];
		walk[r] = temp;
	}
	return walk;
}

/**
 * @brief Generate random message with given length. The random message is vector of int with 0 or 1
 *    @param length length of the random message
 *    @return vector with random message
 */
shared_array<int> randomMessage(int length)
{
	shared_array<int> message(new int[length]);
	for (int i = 0; i < length; i++)
	{
		message[i] = rand() % 2;
	}
	return message;
}