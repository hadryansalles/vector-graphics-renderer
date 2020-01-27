#ifndef RVG_CHRONOS_H
#define RVG_CHRONOS_H

namespace rvg {

class chronos {
public:
    chronos(void);
    void reset(void);
    double elapsed(void);
    double time(void);
private:
    double m_reset;
};

} // namespace rvg

#endif // RVG_CHRONOS_H
