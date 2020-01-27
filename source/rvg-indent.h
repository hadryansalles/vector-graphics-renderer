#ifndef RVG_INDENT_H
#define RVG_INDENT_H

namespace rvg {

class indent {
    int m_n;
    const char *m_v;
public:
    explicit indent(int n = 0, const char *v = "  "): m_n(n), m_v(v) { ; }

    indent operator++(int) { indent x = *this; ++m_n; return x; }

    indent operator--(int) { indent x = *this; --m_n; return x; }

    indent &operator--() { --m_n; return *this; }

    indent &operator++() { ++m_n; return *this; }

    void print(std::ostream &out) const {
        out << '\n';
        for (int i = 0; i < m_n; i++) out << m_v;
    }
};

inline std::ostream &operator<<(std::ostream &out, const indent &id) {
    id.print(out);
    return out;
}

} // namespace rvg

#endif
