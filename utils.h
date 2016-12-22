#ifndef UTILS
#define UTILS

#include <QtMath>

struct Index
{
    int row;
    int column;
};

/*!
 * \brief This function converts standard board index to rectangular type index
 * \param linearIndex Index to convert
 * \return Converted index
 */
Index mapFromLinear(const Index &linearIndex);

/*!
 * \brief This function converts rectangular index to standard board index
 * \param quadraticIndex Index to convert
 * \return Converted index
 */
Index mapToLinear(const Index &quadraticIndex);

#endif // UTILS

