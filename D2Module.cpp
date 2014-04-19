#include "stdafx.h"

#include "D2Module.h"

using namespace Injectag;

D2Module::D2Module()
{
	m_boundryCells[0] = BsrCell(9, CellType::Input, false, true);
	m_boundryCells[1] = BsrCell(8, CellType::Output, 0);
	m_boundryCells[2] = BsrCell(7, CellType::Output, 0);
	m_boundryCells[3] = BsrCell(6, CellType::Output, 0);

	m_boundryCells[4] = BsrCell(5, CellType::Input);
	m_boundryCells[5] = BsrCell(4, CellType::Input);
	m_boundryCells[6] = BsrCell(3, CellType::Input);
	m_boundryCells[7] = BsrCell(2, CellType::Input);
	m_boundryCells[8] = BsrCell(1, CellType::Input);
	m_boundryCells[9] = BsrCell(0, CellType::Input);
}