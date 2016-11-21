#pragma once

#include "Miqui.h"
#include "BaseHeader.h"
#include <miqs>


namespace Miqui
{
namespace Audio
{


class ColorGradient
{
public:
	using ColorType = winrt::Windows::UI::Color;

	ColorType GetColor(double value);

	void AddColor(ColorType color) noexcept;

	void Min(double min) noexcept { m_min = min; }
	double Min() const noexcept { return m_min; }
	void Max(double max) noexcept { m_max = max; }
	double Max() const noexcept { return m_max; }

	void FillWithDefaultColors();

private:
	uint8_t Interpolate(uint8_t start, uint8_t end, double ratio);

	double m_min{ 0.0 }, m_max{ 1.0 };
	std::vector<ColorType> m_colors;
};


inline uint8_t ColorGradient::Interpolate(uint8_t start, uint8_t end, double ratio)
{
	double val = static_cast<double>(end - start) * ratio + static_cast<double>(start);
	return static_cast<uint8_t>(val);
}

template <typename _To>
_To ConvertColor(winrt::Windows::UI::Color const& c)
{
	return _To{ (float)c.R / 255, (float)c.G / 255, (float)c.B / 255 , (float)c.A / 255 };
}



inline ColorGradient::ColorType ColorGradient::GetColor(double value)
{
	if (m_colors.size() <= 1)
	{
		if (m_colors.size() == 0) return ColorType{ 0xff,0,0,0 };
		else return m_colors.back();
	}

	if (value >= m_max)	return m_colors.back();

	double pos = value / m_max * (m_colors.size() - 1);
	int ipos = (int)pos;
	double ratio = pos - ipos;
	if (ipos == m_colors.size() - 1) return m_colors.back();

	ColorType first = m_colors[ipos];
	ColorType second = m_colors[ipos + 1];

	ColorType color;
	color.A = 255;
	color.R = Interpolate(first.R, second.R, ratio);
	color.G = Interpolate(first.G, second.G, ratio);
	color.B = Interpolate(first.B, second.B, ratio);
	return color;
}

inline void ColorGradient::AddColor(ColorType color) noexcept
{ m_colors.push_back(color); }


inline void ColorGradient::FillWithDefaultColors()
{
	m_colors.clear();
	m_colors.push_back({ 0xff,0,0,0 });
	m_colors.push_back({ 0xff,0,0,128 });
	m_colors.push_back({ 0xff,0,128,255 });
	m_colors.push_back({ 0xff,0,255,255 });
	m_colors.push_back({ 0xff,0,255,128 });
	m_colors.push_back({ 0xff,128,255,0 });
	m_colors.push_back({ 0xff,255,128,0 });
	m_colors.push_back({ 0xff,255,0,0 });
}


}
}



