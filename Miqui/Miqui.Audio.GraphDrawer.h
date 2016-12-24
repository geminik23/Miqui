#pragma once
#include "Miqui.Audio.BasicType.h"
#include "Miqui.Audio.AudioGraphControl.h"
namespace Miqui
{
namespace Audio
{

//-------------------------------------------------------------
/* BitmapBasedGraphDrawer */
enum class BitmapBasedGraphDrawMethod
{
	StaticMode,
	RealtimeMode
};

struct BitmapBasedGraphDrawer: BaseGraphDrawer
{
	virtual void CreateDeviceDependentResources(Miqui::D2CanvasEvent&) override;
	void BitmapSize(D2D1_SIZE_F size) noexcept;
	void CreateBitmap(UINT width, UINT height, Miqui::D2CanvasEvent & resource);
	void Reset(D2D1::ColorF const& color);

	BitmapBasedGraphDrawMethod DrawMethod() const noexcept { return m_drawMethod; }
	void DrawMethod(BitmapBasedGraphDrawMethod method) noexcept { m_drawMethod = method; }


	virtual void Draw(Miqui::D2CanvasEvent&) override;


protected:
	BitmapBasedGraphDrawMethod m_drawMethod{ BitmapBasedGraphDrawMethod::StaticMode };
	DrawerInfo m_drawerInfo;
};


/* SpectrogramBitmapBasedDrawer */
struct SpectrogramBitmapBasedDrawer: BitmapBasedGraphDrawer
{
	SpectrogramBitmapBasedDrawer();

	// Inherited via BitmapBasedGraphDrawer
	virtual void Update(Miqui::StepTimer &) override;

	ColorGradient& ColorGradientObject() noexcept { return m_colorGradient; }

private:
	ColorGradient m_colorGradient;
};

/* WaveformBitmapBasedDrawer */
struct WaveformBitmapBasedDrawer: BitmapBasedGraphDrawer
{
	// Inherited via BitmapBasedGraphDrawer
	virtual void Update(Miqui::StepTimer &) override;
};





/**	Todo
	1. Tab size -> rotate? 
	2. Circle size
	3. Conversion -> display x -> sample value method
	4. middle line
	5. Use Virtual Point -> connect? 
*/
//-------------------------------------------------------------
struct EditableWavetable: BaseGraphDrawer
{
	EditableWavetable();

	// Inherited via BaseGraphDrawer
	virtual void CreateDeviceIndependentResources(Miqui::D2CanvasEvent&) override;
	virtual void CreateDeviceDependentResources(Miqui::D2CanvasEvent&) override;
	virtual void Update(Miqui::StepTimer &) override;
	virtual void Draw(Miqui::D2CanvasEvent &) override;
	virtual void OnPointerEvent(Miqui::PointerEvent&) override;

	void TableSize(size_t size) noexcept;
	size_t TableSize() const noexcept;

	std::vector<D2D1_POINT_2F> PointPositions() noexcept;

protected:
	void UpdateYAt(size_t index, float value);

private:
	typedef BaseGraphDrawer base_type;


	bool m_pressed{ false };

	float m_amplitudeRatio{ 0.9f };
	float m_sizeOfControlCircle{ 15.f };
	int m_selectedPoint{ -1 };
};

inline void EditableWavetable::TableSize(size_t size) noexcept
{
	this->m_sampleBuffer.reset(size, 0.f);
}

inline size_t EditableWavetable::TableSize() const noexcept
{
	return m_sampleBuffer.size();
}




}
}