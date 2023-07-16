#include <iostream>
#include <memory>
#include "utils/RustyRingBuffer.hpp"

RustyRingBuffer::RustyRingBuffer(int sizeSamples)                                                   
{
  _data = new float[sizeSamples];

  memset(_data, 1.f, sizeSamples * sizeof(float));
  _size = sizeSamples;
  _readPtr = 0;
  _writePtr = 0;
  _writeSamplesAvail = sizeSamples;

  std::cout << "RustyRingBuffer constructed." << std::endl;
};

RustyRingBuffer::~RustyRingBuffer() 
{
  delete[] _data;
  
  std::cout << "RustyRingBuffer destroyed." << std::endl;
}

bool RustyRingBuffer::empty() 
{
  memset(_data, 0, _size * sizeof(float));
  _readPtr = 0;
  _writePtr = 0;
  _writeSamplesAvail = _size;
  return true;
}

int RustyRingBuffer::read(float* dataPtr, int numSamples)
{
  // If there is no dataPtr, no numSamples to grab or no more samples to read
  // return 0;
  if(dataPtr == 0 || numSamples <= 0 || _writeSamplesAvail == _size)
  {
    return 0;
  }

  // Getting a value for how many samples are available to be read
  int readSamplesAvail = _size - _writeSamplesAvail;

  // Stops us from reading samples ahead of where they've been written
  if(numSamples > readSamplesAvail)
  {
    numSamples = readSamplesAvail;
  }

  if(numSamples > _size - _readPtr)
  {
    int len = (_size-_readPtr) * sizeof(float); // Calculating the length left until the end of the buffer
    memcpy(dataPtr, _data+_readPtr, len); // Reading the memory available until end of buffer
    memcpy(dataPtr+len, _data, numSamples-len); // Wrapping the pointer back around to the start of the buffer then reading the rest of the memory
  }
  else 
  {
    memcpy(dataPtr, _data+_readPtr, numSamples * sizeof(float));
  }

  _readPtr = (_readPtr + numSamples) % _size;
  _writeSamplesAvail += numSamples;

  return numSamples;
}

int RustyRingBuffer::write(float* dataPtr, int numSamples) 
{
  // If there is no dataPtr, no numSamples to grab or no more samples to write to
  // return 0;
  if(dataPtr == 0 || numSamples <= 0 || _writeSamplesAvail == 0) 
  {
    return 0;
  }

  // If we're trying to write to more samples than are available we cap it
  if(numSamples > _writeSamplesAvail)
  {
    numSamples = _writeSamplesAvail;
  }

  if(numSamples > _size - _writePtr)
  {
    int len = (_size - _writePtr) * sizeof(float);
    memcpy(_data+_writePtr, dataPtr, len);
    memcpy(_data, dataPtr+len, numSamples - len);
  }
  else
  {
    memcpy(_data+_writePtr, dataPtr, numSamples * sizeof(float));
  }

  _writePtr = (_writePtr + numSamples) % _size;
  _writeSamplesAvail -= numSamples;

  return numSamples;
}

int RustyRingBuffer::getSize()
{
  return _size;
}

int RustyRingBuffer::getWriteAvail()
{
  return _writeSamplesAvail;
}

int RustyRingBuffer::getReadAvail()
{
  return _size - _writeSamplesAvail;
}