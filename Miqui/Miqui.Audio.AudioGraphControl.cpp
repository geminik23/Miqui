#include "pch.h"
#include "Miqui.Audio.AudioGraphControl.h"

using namespace Miqui::Audio;
using Microsoft::WRL::ComPtr;



inline float GetMeasurementStep(float range)
{
	if(range == 0.f) return 0.f;
	float step{ 1 };
	float s = {};

	if (range >= 1.f)
	{
		s = range / step;
		while ((int)s != 0)
		{
			step *= 10;
			s = range / step;
		}
		step /= 10;

	} else
	{
		range = 1 / range;
		step = 10.f;
		s = range / step;
		while ((int)s != 0)
		{
			step *= 10;
			s = range / step;
		}
		step = 1 / step;
	}
	return step;
}


void AudioGraphControl::CreateDeviceIndependentResources(Miqui::D2CanvasEvent & resource)
{
	base_type::CreateDeviceIndependentResources(resource);
	if (this->m_graphDrawer) m_graphDrawer->CreateDeviceIndependentResources(resource);
}

void AudioGraphControl::CreateDeviceDependentResources(Miqui::D2CanvasEvent &resource)
{
	base_type::CreateDeviceDependentResources(resource);
	if (this->m_graphDrawer) m_graphDrawer->CreateDeviceDependentResources(resource);
}

void AudioGraphControl::GraphDrawer(std::shared_ptr<BaseGraphDrawer> const & drawer)
{ m_graphDrawer = drawer; m_graphDrawer->Parent(this); this->OnSizeChanged(); }

std::shared_ptr<BaseGraphDrawer> AudioGraphControl::GraphDrawer() const noexcept
{ return m_graphDrawer; }

void AudioGraphControl::Update(Miqui::StepTimer& timer)
{
	if (m_graphDrawer) m_graphDrawer->Update(timer);
}

void AudioGraphControl::Render(Miqui::D2CanvasEvent & e)
{
	// draw background
	base_type::Render(e);

	// draw drawer
	if (m_graphDrawer) m_graphDrawer->Draw(e);

	// two Axis
	auto ctx = e.GetD2DDeviceContext();
	auto colorBrush = e.SolidColorBrush();
	ctx->BeginDraw();
	if (m_xlimits.use)
	{
		//TODO



	}

	if (m_ylimits.use)
	{
		// clear
		colorBrush->SetColor(BackgroundColor());

		auto yregion = this->GetRegionOfYLimits();
		auto yfillrect = yregion;
		yfillrect.top -= 1.f;
		yfillrect.bottom += 1.f;
		yfillrect.left -= 1.f;
		yfillrect.right += 1.f;
		auto yr_w = yregion.right - yregion.left;

		ctx->FillRectangle(yfillrect, colorBrush.Get());

		// axis
		colorBrush->SetColor(this->m_gridInfo.gridColor);
		ctx->DrawLine({yregion.right, yregion.top}, {yregion.right, yregion.bottom}, colorBrush.Get());
		

		float yd = m_ylimits.ratio * this->m_ylimits.metro* (yregion.bottom - yregion.top);
		auto count = (int)((yregion.bottom - yregion.top) / yd) + 1;
		float yoffset = (std::fmod(m_ylimits.limits.first , this->m_ylimits.metro)) * m_ylimits.ratio* (yregion.bottom - yregion.top);
		
	
		for (size_t i{}; i < count; i++)
		{
			ctx->DrawLine({ yregion.right - yr_w*0.33f, yoffset }, { yregion.right, yoffset }, colorBrush.Get());
			yoffset += yd;
			if(yoffset > yregion.bottom) break;
		}

		//FIXME 
		
		
		
	}


	ctx->EndDraw();
	
	
}

void AudioGraphControl::OnPointerEvent(Miqui::PointerEvent &pe)
{
	if (m_graphDrawer) m_graphDrawer->OnPointerEvent(pe);
}



void Miqui::Audio::AudioGraphControl::XLimits(std::pair<float, float> const & v) noexcept
{
	if (this->m_xlimits.Update(v))
	{
		OnSizeChanged();
		Invalidate();
	}
}

void Miqui::Audio::AudioGraphControl::YLimits(std::pair<float, float> const & v) noexcept
{
	if (this->m_ylimits.Update(v)) { 
		OnSizeChanged();
		Invalidate(); 
	}
}
//
//void Miqui::Audio::AudioGraphControl::Limits(std::pair<float, float> const & x, std::pair<float, float> const & y) noexcept
//{
//	if(this->m_xlimits == x && this->m_ylimits ==y) return;
//	this->m_xlimits = x;
//	this->m_ylimits = y;
//	this->m_useXLimits = this->m_useYLimits = true;
//	Invalidate();
//}
//
//void Miqui::Audio::AudioGraphControl::HideXLimits() noexcept
//{
//	if(!m_useXLimits) return;
//	m_useXLimits = false; Invalidate();
//}
//
//void Miqui::Audio::AudioGraphControl::HideYLimits() noexcept
//{
//	if(!m_useYLimits) return;
//	m_useYLimits = false; Invalidate();
//}

void AudioGraphControl::OnSizeChanged()
{
	if (this->m_graphDrawer)
	{
		D2D1_RECT_F rect{ EstimateYAxisWidth() ,0.f, Width(), Height() - EstimateXAxisHeight() };
		m_graphDrawer->ControlRect(rect);
	}
}

float Miqui::Audio::AudioGraphControl::EstimateYAxisWidth() noexcept
{
	return (m_ylimits.use )?30.f:0.f;
}

float Miqui::Audio::AudioGraphControl::EstimateXAxisHeight() noexcept
{
	return (m_xlimits.use) ? 30.f : 0.f;
}

D2D1_RECT_F Miqui::Audio::AudioGraphControl::GetRegionOfXLimits() noexcept
{
	return D2D1_RECT_F();
}

D2D1_RECT_F Miqui::Audio::AudioGraphControl::GetRegionOfYLimits() noexcept
{
	auto size = this->Size();
	float h = size.height;
	h -= EstimateXAxisHeight();
	return D2D1::RectF(0.f, 0.f, EstimateYAxisWidth(), h);
}






bool Miqui::Audio::AudioGraphControl::AxisInfo::Update(std::pair<float, float> const & newlimits)
{
	if(this->limits == newlimits) return false;
	this->limits = newlimits;

	auto range = std::fabs(limits.second - limits.first);
	this->ratio = 1.f/range;
	this->metro = GetMeasurementStep(range);
	this->use = true;
	return true;
}


















