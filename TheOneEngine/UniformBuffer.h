#pragma once
#include "Defs.h"

class UniformBuffer
{
public:
	UniformBuffer(uint size, uint binding);
	~UniformBuffer();

	void SetData(const void* data, uint size, uint offset = 0);
private:
	uint m_RendererID = 0;
};
