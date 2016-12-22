#include "utils.h"

Index mapFromLinear(const Index &linearIndex)
{
    const int row = qFloor(linearIndex.row / 3) * 3 + qFloor(linearIndex.column / 3);
    const int column = (linearIndex.row % 3) * 3 + (linearIndex.column % 3);

    return {row, column};
}

Index mapToLinear(const Index &quadraticIndex)
{
    const int row = qFloor(quadraticIndex.row / 3) * 3 + qFloor(quadraticIndex.column / 3);
    const int column = (quadraticIndex.row % 3) * 3 + (quadraticIndex.column % 3);

    return {row, column};
}
