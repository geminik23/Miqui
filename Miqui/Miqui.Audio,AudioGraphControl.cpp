#include "pch.h"
#include "Miqui.Audio.AudioGraphControl.h"

using namespace Miqui::Audio;
using Microsoft::WRL::ComPtr;


void AudioGraphControl::CreateDeviceIndependentResources(Miqui::DeviceResources & resource)
{
	base_type::CreateDeviceIndependentResources(resource);
	if (this->m_graphDrawer) m_graphDrawer->CreateDeviceIndependentResources(resource);
}

void AudioGraphControl::CreateDeviceDependentResources(Miqui::DeviceResources &resource)
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

void AudioGraphControl::Render(Miqui::DeviceResources & resource)
{
	// draw background
	base_type::Render(resource);

	// draw drawer
	if (m_graphDrawer) m_graphDrawer->Draw(resource);

	// TODO Two Axis
}

void AudioGraphControl::OnPointerEvent(Miqui::PointerEvent &pe)
{
	if (m_graphDrawer) m_graphDrawer->OnPointerEvent(pe);
}

void Miqui::Audio::AudioGraphControl::XLimits(std::pair<float, float> const & v) noexcept
{
	if(this->m_xlimits == v) return; 
	this->m_xlimits = v;
	this->m_useXLimits = true;
	this->Invalidate();
}

void Miqui::Audio::AudioGraphControl::YLimits(std::pair<float, float> const & v) noexcept
{
	if (this->m_ylimits == v) return;
	this->m_ylimits = v;
	this->m_useYLimits = true;
	this->Invalidate();
}

void Miqui::Audio::AudioGraphControl::Limits(std::pair<float, float> const & x, std::pair<float, float> const & y) noexcept
{
	if(this->m_xlimits == x && this->m_ylimits ==y) return;
	this->m_xlimits = x;
	this->m_ylimits = y;
	this->m_useXLimits = this->m_useYLimits = true;
	Invalidate();
}

void Miqui::Audio::AudioGraphControl::HideXLimits() noexcept
{
	if(!m_useXLimits) return;
	m_useXLimits = false; Invalidate();
}

void Miqui::Audio::AudioGraphControl::HideYLimits() noexcept
{
	if(!m_useYLimits) return;
	m_useYLimits = false; Invalidate();
}

void AudioGraphControl::OnSizeChanged()
{
	if (this->m_graphDrawer)
	{
		D2D1_RECT_F rect{ 0.f,0.f, Width(), Height() };
		m_graphDrawer->ControlRect(rect);
	}
}
























//---------------------------------------------------implements GraphDrawers

void BitmapBasedGraphDrawer::CreateDeviceDependentResources(Miqui::DeviceResources & resource)
{
	this->CreateBitmap((UINT)m_drawerInfo.bitmapSize.width, (UINT)m_drawerInfo.bitmapSize.height, resource);
	this->Reset(D2D1::ColorF{ D2D1::ColorF::Black });
}

void BitmapBasedGraphDrawer::BitmapSize(D2D1_SIZE_F size) noexcept
{
	this->m_drawerInfo.bitmapSize = size;
}

void BitmapBasedGraphDrawer::CreateBitmap(UINT width, UINT height, Miqui::DeviceResources & resource)
{
	auto factory = resource.GetD2DFactory();
	auto imgFactory = resource.GetWicImagingFactory();
	DxBase::ThrowIfFailed(imgFactory->CreateBitmap(width, height,
												   GUID_WICPixelFormat32bppPBGRA,
												   WICBitmapCacheOnLoad,
												   m_drawerInfo.bitmap.ReleaseAndGetAddressOf()));

	DxBase::ThrowIfFailed(factory->CreateWicBitmapRenderTarget(m_drawerInfo.bitmap.Get(),
															   D2D1::RenderTargetProperties(),
															   m_drawerInfo.target.ReleaseAndGetAddressOf()));
	m_drawerInfo.target->CreateSolidColorBrush({}, &this->m_drawerInfo.brush);
}

void BitmapBasedGraphDrawer::Reset(D2D1::ColorF const & color)
{
	if (!m_drawerInfo.bitmap) return;

	UINT w, h;
	this->m_drawerInfo.bitmap->GetSize(&w, &h);

	D2D1_RECT_F rect;
	rect.left = -1.f;
	rect.top = -1.f;
	rect.right = w + 1.f;
	rect.bottom = h + 1.f;

	m_drawerInfo.target->BeginDraw();
	m_drawerInfo.brush->SetColor(color);
	m_drawerInfo.target->FillRectangle(rect, m_drawerInfo.brush.Get());
	m_drawerInfo.target->EndDraw();

	// reset -> drawInfo
	m_drawerInfo.currentCount = 0;
}

void BitmapBasedGraphDrawer::Draw(Miqui::DeviceResources & resource)
{
	if (!m_drawerInfo.bitmap) return;


	D2D1_RECT_F rect = this->ControlRect();
	rect.top -= 1.f;
	rect.bottom += 2.f;

	if (m_drawMethod == BitmapBasedGraphDrawMethod::RealtimeMode)
	{
		auto ctx = resource.GetD2DDeviceContext();

		float width = rect.right - rect.left;
		//float each_width = width / (this->m_span_circular.size - 1);
		UINT s[2];
		m_drawerInfo.bitmap->GetSize(&s[0], &s[1]);
		float each_delta = width / (float)s[0];
		rect.left -= (m_drawerInfo.currentCount*each_delta);

		rect.right = rect.left + width;

		float l = rect.left;
		ComPtr<ID2D1Bitmap> _bitmap;
		ctx->CreateBitmapFromWicBitmap(m_drawerInfo.bitmap.Get(), _bitmap.ReleaseAndGetAddressOf());

		ctx->BeginDraw();
		for (size_t i{}; i < 2; ++i)
		{
			ctx->DrawBitmap(_bitmap.Get(), rect);

			l += width;
			rect.left = l - 1;
			rect.right = l + width + 1;
		}
		ctx->EndDraw();
	} else
	{
		auto ctx = resource.GetD2DDeviceContext();

		rect.left -= 1.f;
		rect.right += 1.f;

		ComPtr<ID2D1Bitmap> _bitmap;
		ctx->CreateBitmapFromWicBitmap(m_drawerInfo.bitmap.Get(), _bitmap.ReleaseAndGetAddressOf());
		ctx->BeginDraw();
		ctx->DrawBitmap(_bitmap.Get(), rect);
		ctx->EndDraw();
	}
}




SpectrogramBitmapBasedDrawer::SpectrogramBitmapBasedDrawer()
{ this->m_colorGradient.FillWithDefaultColors(); }

void SpectrogramBitmapBasedDrawer::Update(Miqui::StepTimer &)
{
	if (!this->m_graphSampleHandler) return;
	auto b_ptr = miqs::ptr_begin(m_sampleBuffer);
	auto e_ptr = miqs::ptr_end(m_sampleBuffer);
	while (this->m_graphSampleHandler->HandleSamples(b_ptr, e_ptr, GraphSampleHandlerMode::Pull))
	{
		UINT w, h;

		auto count = m_sampleBuffer.size();
		this->m_drawerInfo.bitmap->GetSize(&w, &h);
		float yd = h / count;

		D2D1_RECT_F rect;
		rect.left = m_drawerInfo.currentCount - 1;
		rect.top = h - yd;
		rect.right = m_drawerInfo.currentCount + 2;
		rect.bottom = h;


		m_drawerInfo.target->BeginDraw();
		auto first = b_ptr;
		for (; first != e_ptr; )
		{
			m_drawerInfo.brush->SetColor(ConvertColor<D2D1::ColorF>(m_colorGradient.GetColor(*first++)));

			m_drawerInfo.target->FillRectangle(rect, m_drawerInfo.brush.Get());
			rect.top -= yd;
			rect.bottom -= yd;
		}

		m_drawerInfo.target->EndDraw();


		if (++m_drawerInfo.currentCount >= (w))
		{
			m_drawerInfo.currentCount = 0;
		}

	}


}

void WaveformBitmapBasedDrawer::Update(Miqui::StepTimer &)
{
	if (!this->m_graphSampleHandler) return;
	auto b_ptr = miqs::ptr_begin(m_sampleBuffer);
	auto e_ptr = miqs::ptr_end(m_sampleBuffer);
	while (this->m_graphSampleHandler->HandleSamples(b_ptr, e_ptr, GraphSampleHandlerMode::Pull))
	{
		UINT w, h;

		this->m_drawerInfo.bitmap->GetSize(&w, &h);
		float half = h / 2;

		D2D1_RECT_F rect;
		rect.left = this->m_drawerInfo.currentCount;
		rect.right = this->m_drawerInfo.currentCount + 1;
		rect.top = h;
		rect.bottom = 0;

		this->m_drawerInfo.target->BeginDraw();

		this->m_drawerInfo.brush->SetColor(D2D1::ColorF{ 0x000000 });
		this->m_drawerInfo.target->FillRectangle(rect, this->m_drawerInfo.brush.Get());

		D2D1::ColorF r{ 0xff0000, 0.7f };
		this->m_drawerInfo.brush->SetColor(r);

		float mmmm[4] = { 0.f,0.f,0.f,0.f }; // pos_max rms, neg_max rms
		int count[2] = { 0,0 };
		auto first = b_ptr;
		for (; first != e_ptr; ++first)
		{
			if (*first > 0)
			{
				count[0]++;
				mmmm[1] += (*first * *first);
				if (*first > mmmm[0]) mmmm[0] = *first;
			} else
			{
				count[1]++;
				mmmm[2] += (*first * *first);
				if (*first < mmmm[3]) mmmm[3] = *first;
			}
		}
		mmmm[1] /= (float)count[0];
		mmmm[1] = std::sqrtf(mmmm[1]);
		mmmm[2] /= (float)count[1];
		mmmm[2] = -1.f*std::sqrtf(mmmm[2]);


		//


		rect.top = half;
		rect.bottom = half;

		for (size_t i{}; i < 4; ++i)
		{
			rect.top = half + 0.9f*half*mmmm[i];
			this->m_drawerInfo.target->FillRectangle(rect, this->m_drawerInfo.brush.Get());
		}

		this->m_drawerInfo.target->EndDraw();


		if (++this->m_drawerInfo.currentCount >= (w))
		{
			this->m_drawerInfo.currentCount = 0;
		}
	}
}

void BaseGraphDrawer::ResizeBuffer(size_t size)
{
	m_sampleBuffer.resize(size);
}

void BaseGraphDrawer::Invalidate() noexcept
{
	if (m_parent) m_parent->Invalidate();
}














//--------------------Editable Wavetable

EditableWavetable::EditableWavetable()
{
	this->TableSize(16);
}

void EditableWavetable::CreateDeviceIndependentResources(Miqui::DeviceResources & resource)
{
	base_type::CreateDeviceIndependentResources(resource);
}

void EditableWavetable::CreateDeviceDependentResources(Miqui::DeviceResources & resource)
{
	base_type::CreateDeviceDependentResources(resource);
}

void EditableWavetable::Update(Miqui::StepTimer &)
{}

void EditableWavetable::Draw(Miqui::DeviceResources & resource)
{
	auto ctx = resource.GetD2DDeviceContext();

	//auto rect = this->ControlRect();
	auto brush = Parent()->SolidColorBrush();
	auto positions = this->PointPositions();


	// FIXME Draw default linear mode regarding of GraphDrawingModeType
	//auto drawingMode = this->Parent()->GraphDrawingMode();

	ctx->BeginDraw();

	// Color
	D2D1::ColorF circleColor{ D2D1::ColorF::LightCyan, 0.6f };
	D2D1::ColorF selCircleColor{ D2D1::ColorF::Red, 0.6f };

	// line drawing with geometries

	Microsoft::WRL::ComPtr<ID2D1PathGeometry> pPathGeometry;
	DxBase::ThrowIfFailed(resource.GetD2DFactory()->CreatePathGeometry(&pPathGeometry));
	ComPtr<ID2D1GeometrySink> pSink;

	DxBase::ThrowIfFailed(pPathGeometry->Open(&pSink));

	pSink->BeginFigure(
		positions[0],
		D2D1_FIGURE_BEGIN_FILLED);
	pSink->AddLines(positions.data() + 1, positions.size() - 1);
	pSink->EndFigure(D2D1_FIGURE_END_OPEN);
	pSink->Close();


	// draw lines
	brush->SetColor(Parent()->ForegroundColor());
	ctx->DrawGeometry(pPathGeometry.Get(), brush.Get());


	for (size_t i{}; i < positions.size(); ++i)
	{
		brush->SetColor(((int)i == m_selectedPoint) ? selCircleColor : circleColor);

		ctx->FillEllipse(D2D1::Ellipse(positions[i], this->m_sizeOfControlCircle, this->m_sizeOfControlCircle), brush.Get());
	}

	ctx->EndDraw();
}

std::vector<D2D1_POINT_2F> EditableWavetable::PointPositions() noexcept
{
	auto size = TableSize();
	auto rect = this->ControlRect();
	auto halfHeight = (rect.bottom - rect.top)*0.5f;

	std::vector<D2D1_POINT_2F> result(this->TableSize());

	auto dx = (rect.right - rect.left) / (size - 1);

	float px{ rect.left };
	for (size_t i{}; i < size; ++i, px += dx)
	{
		result[i].x = px;
		result[i].y = (-this->m_sampleBuffer[i] * m_amplitudeRatio) * (halfHeight - m_sizeOfControlCircle) + halfHeight + rect.top;
	}
	return result;
}


void EditableWavetable::UpdateYAt(size_t index, float value)
{
	auto rect = this->ControlRect();
	auto halfHeight = (rect.bottom - rect.top)*0.5f;


	value = miqs::clipping<float>(rect.bottom - m_sizeOfControlCircle, rect.top + m_sizeOfControlCircle)(value);
	//std::wostringstream ss;
	//ss<<value<<L"\n";
	//OutputDebugString(ss.str().c_str());

	m_sampleBuffer[index] = (value - halfHeight) / ((halfHeight - m_sizeOfControlCircle)) * -1.f;
}

bool _IsInsideEllipse(D2D1_POINT_2F center, float radius, D2D1_POINT_2F point)
{
	return  (point.x >= center.x - radius && point.x < center.x + radius) &&
		(point.y >= center.y - radius && point.y < center.y + radius);
}

void EditableWavetable::OnPointerEvent(Miqui::PointerEvent &e)
{
	auto ppos = e.GetCurrentPoint().Position();
	auto update = bool{ false };

	switch (e.Mode())
	{
	case Miqui::PointerMode::Presssed:
		//checkPosition
		{
			auto positions = PointPositions();
			for (size_t i{}; i < positions.size(); ++i)
			{
				if (_IsInsideEllipse(positions[i], this->m_sizeOfControlCircle, ppos))
				{
					m_selectedPoint = i;
					break;
				}
			}
		}
		if (m_selectedPoint != -1)
		{
			m_pressed = true;
			e.Handled(true);
		}
		break;
	case Miqui::PointerMode::Moved:
		if (m_pressed)
		{
			e.Handled(true);
			this->UpdateYAt(m_selectedPoint, ppos.y);
			update = true;
		}
		break;
	case Miqui::PointerMode::Released:
		if (m_pressed)
		{
			this->UpdateYAt(m_selectedPoint, ppos.y);
			update = true;
		}
	case Miqui::PointerMode::Canceled:
	case Miqui::PointerMode::Exited:
		m_pressed = false;
		m_selectedPoint = -1;
		break;
	}

	if (update)
	{
		GraphSampleHandler(miqs::ptr_begin(m_sampleBuffer), miqs::ptr_end(m_sampleBuffer), GraphSampleHandlerMode::Push);
		this->Invalidate();
	}
}


