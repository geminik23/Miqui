#pragma once
#include "Miqui.Audio.BasicType.h"

namespace Miqui
{
namespace Audio
{



/* DrawerInfo for DrawMethod */
struct DrawerInfo
{
	Microsoft::WRL::ComPtr<IWICBitmap> bitmap;
	Microsoft::WRL::ComPtr<ID2D1RenderTarget> target;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;

	D2D1_SIZE_F bitmapSize;
	int currentCount;
};

// States
enum class GraphDrawingModeType
{
	Linear,
	Stem,

};


//
//==================== Interface

enum class GraphSampleHandlerMode
{
	Pull,
	Peek,
	Push
};

/* Graph Sample Handler*/
template <typename _Ty>
struct IGraphSampleHandler
{
	virtual bool HandleSamples(_Ty * first, _Ty * last, GraphSampleHandlerMode mode) = 0;
};



//===========================================Graph Drawer's
/* GraphDrawer */
class AudioGraphControl;
struct BaseGraphDrawer
{
	virtual void Update(Miqui::StepTimer&) = 0;
	virtual void Draw(Miqui::D2CanvasEvent&) = 0;

	virtual void CreateDeviceIndependentResources(Miqui::D2CanvasEvent&) {}
	virtual void CreateDeviceDependentResources(Miqui::D2CanvasEvent&) {}
	virtual void OnPointerEvent(Miqui::PointerEvent&) {}

	void GraphSampleHandler(IGraphSampleHandler<float> * handler) noexcept { m_graphSampleHandler = handler; }
	IGraphSampleHandler<float> * GraphSampleHandler() const noexcept { return m_graphSampleHandler; }
	void GraphSampleHandler(float* f, float*l, GraphSampleHandlerMode mode) noexcept { if (m_graphSampleHandler) m_graphSampleHandler->HandleSamples(f, l, mode); }
	void GraphSampleHandler(GraphSampleHandlerMode mode) noexcept { if (m_graphSampleHandler) m_graphSampleHandler->HandleSamples(std::begin(m_sampleBuffer), std::end(m_sampleBuffer), mode); }

	void ControlRect(D2D1_RECT_F rect) noexcept;
	D2D1_RECT_F ControlRect() const noexcept;

	void ResizeBuffer(size_t size);
	miqs::array_ref<float> Data() noexcept{ return m_sampleBuffer;}

	void Parent(AudioGraphControl * parent) noexcept { m_parent = parent; }
	AudioGraphControl* Parent() const noexcept { return m_parent; }

	void Invalidate() noexcept;


protected:
	AudioGraphControl * m_parent;
	IGraphSampleHandler<float> * m_graphSampleHandler{ nullptr };
	miqs::array<float> m_sampleBuffer;
	D2D1_RECT_F m_controlRect{};
};

inline void BaseGraphDrawer::ControlRect(D2D1_RECT_F rect) noexcept { this->m_controlRect = rect; }
inline D2D1_RECT_F BaseGraphDrawer::ControlRect() const noexcept { return this->m_controlRect; }








//
//---------------------------------------------
/* AudioGraphControl */
class AudioGraphControl: public Miqui::IControl
{
	typedef Miqui::IControl base_type;
public:
	virtual void CreateDeviceIndependentResources(Miqui::D2CanvasEvent&) override;
	virtual void CreateDeviceDependentResources(Miqui::D2CanvasEvent&) override;
	//virtual void CreateWindowSizeDependentResources(Miqui::D2CanvasEvent&) override {}
	//virtual void ReleaseDeviceDependentResources(Miqui::D2CanvasEvent&) override {}
	//virtual void OnColorProfileChanged(winrt::Windows::Graphics::Display::DisplayInformation const &) override {}

	virtual void Update(Miqui::StepTimer&) override;
	virtual void Render(Miqui::D2CanvasEvent&) override;

	void GraphDrawer(std::shared_ptr<BaseGraphDrawer> const& drawer);
	std::shared_ptr<BaseGraphDrawer> GraphDrawer() const noexcept;

	virtual void OnPointerEvent(Miqui::PointerEvent&) override;

	GraphDrawingModeType GraphDrawingMode() const noexcept { return m_graphDrawingMode; }
	void GraphDrawingMode(GraphDrawingModeType mode) noexcept { m_graphDrawingMode = mode; }

	////---------------limits

	//void SampleInfo(size_t count, float start = 0.f, float step = 1.f, bool updateYLimits) noexcept;
	//std::pair<float, float> XLimits() const noexcept { return m_xlimits; }
	//std::pair<float, float> YLimits() const noexcept { return m_ylimits; }

	void XLimits(std::pair<float, float> const& v) noexcept;
	void YLimits(std::pair<float, float> const& v) noexcept;
	//void Limits(std::pair<float, float> const& x, std::pair<float, float> const& y) noexcept;

	//bool UseXLimits() const noexcept { return m_useXLimits; }
	//bool UseYLimits() const noexcept { return m_useYLimits; }
	//
	//void HideXLimits() noexcept;
	//void HideYLimits() noexcept;

protected:
	virtual void OnSizeChanged() override;

	float EstimateYAxisWidth() noexcept;
	float EstimateXAxisHeight() noexcept;

	D2D1_RECT_F GetRegionOfXLimits() noexcept;
	D2D1_RECT_F GetRegionOfYLimits() noexcept;

private:
	std::shared_ptr<BaseGraphDrawer> m_graphDrawer;
	GraphDrawingModeType m_graphDrawingMode{ GraphDrawingModeType::Linear };




	struct AxisInfo
	{
		bool use{ false };
		std::pair<float, float> limits{};
		float metro{ 1.f };
		float ratio{};

		bool Update(std::pair<float, float> const& newlimits);
	};

	struct GridInfo
	{
		D2D1_COLOR_F gridColor{ D2D1::ColorF(0xffffff, 1.f) };
	};

	AxisInfo m_xlimits, m_ylimits;
	GridInfo m_gridInfo;
};


}
}