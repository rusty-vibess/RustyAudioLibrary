#ifndef RUSTY_RING_BUFFER_HPP
#define RUSTY_RING_BUFFER_HPP

class RustyRingBuffer 
{
  public:
    RustyRingBuffer( int sizeSamples );
    ~RustyRingBuffer();

    int read( float* dataPtr, int numSamples );
    int write( float* dataPtr, int numSamples );
    bool empty();
    int getSize();
    int getWriteAvail();
    int getReadAvail();

  private:
    float* _data;
    int _size;
    int _readPtr;
    int _writePtr;
    int _writeSamplesAvail;
};

#endif