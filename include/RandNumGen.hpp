#ifndef RandNumGen_HPP
#define RandNumGen_HPP

class RandNumGen {
public:
    virtual ~RandNumGen() = default;
    virtual double GenerateRandNum() = 0;
};

#endif
