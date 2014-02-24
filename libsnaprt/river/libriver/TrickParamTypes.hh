#ifndef TRICKPARAMTYPES
#define TRICKPARAMTYPES

// Trick parameter types interface for fields in .trk files
class TrickParamTypes
{
  public:
    TrickParamTypes(int swap): _swap(swap) {}
    virtual double toValue(void* address, int type, int size) = 0;
    virtual int doubleType() const = 0;
  protected:
    int _swap; ///< Whether parameters are byteswapped
};

#endif
