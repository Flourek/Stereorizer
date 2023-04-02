//// [src] https://github.com/ocornut/imgui/issues/123
//// [src] https://github.com/ocornut/imgui/issues/55
//
//// v1.22 - flip button; cosmetic fixes
//// v1.21 - oops :)
//// v1.20 - add iq's interpolation code
//// v1.10 - easing and colors
//// v1.00 - jari komppa's original
//
//#pragma once
//
//#define IMGUI_DEFINE_MATH_OPERATORS
//#include "imgui.h"
//#include "imgui_internal.h"
//
//#include <cmath>
//
///* To use, add this prototype somewhere..
//
//namespace ImGui
//{
//    int Curve(const char *label, const ImVec2& size, int maxpoints, ImVec2 *points);
//    float CurveValue(float p, int maxpoints, const ImVec2 *points);
//    float CurveValueSmooth(float p, int maxpoints, const ImVec2 *points);
//};
//
//*/
///*
//    Example of use:
//
//    ImVec2 foo[10];
//    ...
//    foo[0].x = -1; // init data so editor knows to take it from here
//    ...
//    if (ImGui::Curve("Das editor", ImVec2(600, 200), 10, foo))
//    {
//        // curve changed
//    }
//    ...
//    float value_you_care_about = ImGui::CurveValue(0.7f, 10, foo); // calculate value at position 0.7
//*/
//
//namespace tween {
//    enum TYPE
//    {
//        LINEAR,
//
//        QUADIN,          // t^2
//        QUADOUT,
//        QUADINOUT,
//        CUBICIN,         // t^3
//        CUBICOUT,
//        CUBICINOUT,
//        QUARTIN,         // t^4
//        QUARTOUT,
//        QUARTINOUT,
//        QUINTIN,         // t^5
//        QUINTOUT,
//        QUINTINOUT,
//        SINEIN,          // sin(t)
//        SINEOUT,
//        SINEINOUT,
//        EXPOIN,          // 2^t
//        EXPOOUT,
//        EXPOINOUT,
//        CIRCIN,          // sqrt(1-t^2)
//        CIRCOUT,
//        CIRCINOUT,
//        ELASTICIN,       // exponentially decaying sine wave
//        ELASTICOUT,
//        ELASTICINOUT,
//        BACKIN,          // overshooting cubic easing: (s+1)*t^3 - s*t^2
//        BACKOUT,
//        BACKINOUT,
//        BOUNCEIN,        // exponentially decaying parabolic bounce
//        BOUNCEOUT,
//        BOUNCEINOUT,
//
//        SINESQUARE,      // gapjumper's
//        EXPONENTIAL,     // gapjumper's
//        SCHUBRING1,      // terry schubring's formula 1
//        SCHUBRING2,      // terry schubring's formula 2
//        SCHUBRING3,      // terry schubring's formula 3
//
//        SINPI2,          // tomas cepeda's
//        SWING,           // tomas cepeda's & lquery's
//    };
//
//    // }
//
//    // implementation
//
//    static inline
//    double ease( int easetype, double t )
//    {
//        using namespace std;
//
//        const double d = 1.f;
//        const double pi = 3.1415926535897932384626433832795;
//        const double pi2 = 3.1415926535897932384626433832795 / 2;
//
//        double p = t/d;
//
//        switch( easetype )
//        {
//            // Modeled after the line y = x
//            default:
//            case TYPE::LINEAR: {
//                return p;
//            }
//
//                // Modeled after the parabola y = x^2
//            case TYPE::QUADIN: {
//                return p * p;
//            }
//
//                // Modeled after the parabola y = -x^2 + 2x
//            case TYPE::QUADOUT: {
//                return -(p * (p - 2));
//            }
//
//                // Modeled after the piecewise quadratic
//                // y = (1/2)((2x)^2)             ; [0, 0.5)
//                // y = -(1/2)((2x-1)*(2x-3) - 1) ; [0.5, 1]
//            case TYPE::QUADINOUT: {
//                if(p < 0.5) {
//                    return 2 * p * p;
//                }
//                else {
//                    return (-2 * p * p) + (4 * p) - 1;
//                }
//            }
//
//                // Modeled after the cubic y = x^3
//            case TYPE::CUBICIN: {
//                return p * p * p;
//            }
//
//                // Modeled after the cubic y = (x - 1)^3 + 1
//            case TYPE::CUBICOUT: {
//                double f = (p - 1);
//                return f * f * f + 1;
//            }
//
//                // Modeled after the piecewise cubic
//                // y = (1/2)((2x)^3)       ; [0, 0.5)
//                // y = (1/2)((2x-2)^3 + 2) ; [0.5, 1]
//            case TYPE::CUBICINOUT: {
//                if(p < 0.5) {
//                    return 4 * p * p * p;
//                }
//                else {
//                    double f = ((2 * p) - 2);
//                    return 0.5 * f * f * f + 1;
//                }
//            }
//
//                // Modeled after the quartic x^4
//            case TYPE::QUARTIN: {
//                return p * p * p * p;
//            }
//
//                // Modeled after the quartic y = 1 - (x - 1)^4
//            case TYPE::QUARTOUT: {
//                double f = (p - 1);
//                return f * f * f * (1 - p) + 1;
//            }
//
//                // Modeled after the piecewise quartic
//                // y = (1/2)((2x)^4)        ; [0, 0.5)
//                // y = -(1/2)((2x-2)^4 - 2) ; [0.5, 1]
//            case TYPE::QUARTINOUT:  {
//                if(p < 0.5) {
//                    return 8 * p * p * p * p;
//                }
//                else {
//                    double f = (p - 1);
//                    return -8 * f * f * f * f + 1;
//                }
//            }
//
//                // Modeled after the quintic y = x^5
//            case TYPE::QUINTIN:  {
//                return p * p * p * p * p;
//            }
//
//                // Modeled after the quintic y = (x - 1)^5 + 1
//            case TYPE::QUINTOUT:  {
//                double f = (p - 1);
//                return f * f * f * f * f + 1;
//            }
//
//                // Modeled after the piecewise quintic
//                // y = (1/2)((2x)^5)       ; [0, 0.5)
//                // y = (1/2)((2x-2)^5 + 2) ; [0.5, 1]
//            case TYPE::QUINTINOUT:  {
//                if(p < 0.5) {
//                    return 16 * p * p * p * p * p;
//                }
//                else {
//                    double f = ((2 * p) - 2);
//                    return  0.5 * f * f * f * f * f + 1;
//                }
//            }
//
//                // Modeled after quarter-cycle of sine wave
//            case TYPE::SINEIN: {
//                return sin((p - 1) * pi2) + 1;
//            }
//
//                // Modeled after quarter-cycle of sine wave (different phase)
//            case TYPE::SINEOUT: {
//                return sin(p * pi2);
//            }
//
//                // Modeled after half sine wave
//            case TYPE::SINEINOUT: {
//                return 0.5 * (1 - cos(p * pi));
//            }
//
//                // Modeled after shifted quadrant IV of unit circle
//            case TYPE::CIRCIN: {
//                return 1 - sqrt(1 - (p * p));
//            }
//
//                // Modeled after shifted quadrant II of unit circle
//            case TYPE::CIRCOUT: {
//                return sqrt((2 - p) * p);
//            }
//
//                // Modeled after the piecewise circular function
//                // y = (1/2)(1 - sqrt(1 - 4x^2))           ; [0, 0.5)
//                // y = (1/2)(sqrt(-(2x - 3)*(2x - 1)) + 1) ; [0.5, 1]
//            case TYPE::CIRCINOUT: {
//                if(p < 0.5) {
//                    return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
//                }
//                else {
//                    return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
//                }
//            }
//
//                // Modeled after the exponential function y = 2^(10(x - 1))
//            case TYPE::EXPOIN: {
//                return (p == 0.0) ? p : pow(2, 10 * (p - 1));
//            }
//
//                // Modeled after the exponential function y = -2^(-10x) + 1
//            case TYPE::EXPOOUT: {
//                return (p == 1.0) ? p : 1 - pow(2, -10 * p);
//            }
//
//                // Modeled after the piecewise exponential
//                // y = (1/2)2^(10(2x - 1))         ; [0,0.5)
//                // y = -(1/2)*2^(-10(2x - 1))) + 1 ; [0.5,1]
//            case TYPE::EXPOINOUT: {
//                if(p == 0.0 || p == 1.0) return p;
//
//                if(p < 0.5) {
//                    return 0.5 * pow(2, (20 * p) - 10);
//                }
//                else {
//                    return -0.5 * pow(2, (-20 * p) + 10) + 1;
//                }
//            }
//
//                // Modeled after the damped sine wave y = sin(13pi/2*x)*pow(2, 10 * (x - 1))
//            case TYPE::ELASTICIN: {
//                return sin(13 * pi2 * p) * pow(2, 10 * (p - 1));
//            }
//
//                // Modeled after the damped sine wave y = sin(-13pi/2*(x + 1))*pow(2, -10x) + 1
//            case TYPE::ELASTICOUT: {
//                return sin(-13 * pi2 * (p + 1)) * pow(2, -10 * p) + 1;
//            }
//
//                // Modeled after the piecewise exponentially-damped sine wave:
//                // y = (1/2)*sin(13pi/2*(2*x))*pow(2, 10 * ((2*x) - 1))      ; [0,0.5)
//                // y = (1/2)*(sin(-13pi/2*((2x-1)+1))*pow(2,-10(2*x-1)) + 2) ; [0.5, 1]
//            case TYPE::ELASTICINOUT: {
//                if(p < 0.5) {
//                    return 0.5 * sin(13 * pi2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
//                }
//                else {
//                    return 0.5 * (sin(-13 * pi2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
//                }
//            }
//
//                // Modeled (originally) after the overshooting cubic y = x^3-x*sin(x*pi)
//            case TYPE::BACKIN: { /*
//                return p * p * p - p * sin(p * pi); */
//                double s = 1.70158f;
//                return p * p * ((s + 1) * p - s);
//            }
//
//                // Modeled (originally) after overshooting cubic y = 1-((1-x)^3-(1-x)*sin((1-x)*pi))
//            case TYPE::BACKOUT: { /*
//                double f = (1 - p);
//                return 1 - (f * f * f - f * sin(f * pi)); */
//                double s = 1.70158f;
//                return --p, 1.f * (p*p*((s+1)*p + s) + 1);
//            }
//
//                // Modeled (originally) after the piecewise overshooting cubic function:
//                // y = (1/2)*((2x)^3-(2x)*sin(2*x*pi))           ; [0, 0.5)
//                // y = (1/2)*(1-((1-x)^3-(1-x)*sin((1-x)*pi))+1) ; [0.5, 1]
//            case TYPE::BACKINOUT: { /*
//                if(p < 0.5) {
//                    double f = 2 * p;
//                    return 0.5 * (f * f * f - f * sin(f * pi));
//                }
//                else {
//                    double f = (1 - (2*p - 1));
//                    return 0.5 * (1 - (f * f * f - f * sin(f * pi))) + 0.5;
//                } */
//                double s = 1.70158f * 1.525f;
//                if (p < 0.5) {
//                    return p *= 2, 0.5 * p * p * (p*s+p-s);
//                }
//                else {
//                    return p = p * 2 - 2, 0.5 * (2 + p*p*(p*s+p+s));
//                }
//            }
//
//#           define tween$bounceout(p) ( \
//                (p) < 4/11.0 ? (121 * (p) * (p))/16.0 : \
//                (p) < 8/11.0 ? (363/40.0 * (p) * (p)) - (99/10.0 * (p)) + 17/5.0 : \
//                (p) < 9/10.0 ? (4356/361.0 * (p) * (p)) - (35442/1805.0 * (p)) + 16061/1805.0 \
//                           : (54/5.0 * (p) * (p)) - (513/25.0 * (p)) + 268/25.0 )
//
//            case TYPE::BOUNCEIN: {
//                return 1 - tween$bounceout(1 - p);
//            }
//
//            case TYPE::BOUNCEOUT: {
//                return tween$bounceout(p);
//            }
//
//            case TYPE::BOUNCEINOUT: {
//                if(p < 0.5) {
//                    return 0.5 * (1 - tween$bounceout(1 - p * 2));
//                }
//                else {
//                    return 0.5 * tween$bounceout((p * 2 - 1)) + 0.5;
//                }
//            }
//
//#           undef tween$bounceout
//
//            case TYPE::SINESQUARE: {
//                double A = sin((p)*pi2);
//                return A*A;
//            }
//
//            case TYPE::EXPONENTIAL: {
//                return 1/(1+exp(6-12*(p)));
//            }
//
//            case TYPE::SCHUBRING1: {
//                return 2*(p+(0.5f-p)*abs(0.5f-p))-0.5f;
//            }
//
//            case TYPE::SCHUBRING2: {
//                double p1pass= 2*(p+(0.5f-p)*abs(0.5f-p))-0.5f;
//                double p2pass= 2*(p1pass+(0.5f-p1pass)*abs(0.5f-p1pass))-0.5f;
//                double pAvg=(p1pass+p2pass)/2;
//                return pAvg;
//            }
//
//            case TYPE::SCHUBRING3: {
//                double p1pass= 2*(p+(0.5f-p)*abs(0.5f-p))-0.5f;
//                double p2pass= 2*(p1pass+(0.5f-p1pass)*abs(0.5f-p1pass))-0.5f;
//                return p2pass;
//            }
//
//            case TYPE::SWING: {
//                return ((-cos(pi * p) * 0.5) + 0.5);
//            }
//
//            case TYPE::SINPI2: {
//                return sin(p * pi2);
//            }
//        }
//    }
//}
//
//namespace ImGui
//{
//    // [src] http://iquilezles.org/www/articles/minispline/minispline.htm
//    // key format (for dim == 1) is (t0,x0,t1,x1 ...)
//    // key format (for dim == 2) is (t0,x0,y0,t1,x1,y1 ...)
//    // key format (for dim == 3) is (t0,x0,y0,z0,t1,x1,y1,z1 ...)
//    void spline( const float *key, int num, int dim, float t, float *v )
//    {
//        static signed char coefs[16] = {
//                -1, 2,-1, 0,
//                3,-5, 0, 2,
//                -3, 4, 1, 0,
//                1,-1, 0, 0 };
//
//        const int size = dim + 1;
//
//        // find key
//        int k = 0; while( key[k*size] < t ) k++;
//
//        // interpolant
//        const float h = (t-key[(k-1)*size])/(key[k*size]-key[(k-1)*size]);
//
//        // init result
//        for( int i=0; i < dim; i++ ) v[i] = 0.0f;
//
//        // add basis functions
//        for( int i=0; i<4; i++ )
//        {
//            int kn = k+i-2; if( kn<0 ) kn=0; else if( kn>(num-1) ) kn=num-1;
//
//            const signed char *co = coefs + 4*i;
//
//            const float b  = 0.5f*(((co[0]*h + co[1])*h + co[2])*h + co[3]);
//
//            for( int j=0; j < dim; j++ ) v[j] += b * key[kn*size+j+1];
//        }
//    }
//
//    float CurveValueSmooth(float p, int maxpoints, const ImVec2 *points)
//    {
//        if (maxpoints < 2 || points == 0)
//            return 0;
//        if (p < 0) return points[0].y;
//
//        float *input = new float [ maxpoints * 2 ];
//        float output[4];
//
//        for( int i = 0; i < maxpoints; ++i ) {
//            input[ i * 2 + 0 ] = points[i].x;
//            input[ i * 2 + 1 ] = points[i].y;
//        }
//
//        spline( input, maxpoints, 1, p, output );
//
//        delete [] input;
//        return output[0];
//    }
//
//    float CurveValue(float p, int maxpoints, const ImVec2 *points)
//    {
//        if (maxpoints < 2 || points == 0)
//            return 0;
//        if (p < 0) return points[0].y;
//
//        int left = 0;
//        while (left < maxpoints && points[left].x < p && points[left].x != -1) left++;
//        if (left) left--;
//
//        if (left == maxpoints-1)
//            return points[maxpoints - 1].y;
//
//        float d = (p - points[left].x) / (points[left + 1].x - points[left].x);
//
//        return points[left].y + (points[left + 1].y - points[left].y) * d;
//    }
//
//    int Curve(const char *label, const ImVec2& size, const int maxpoints, ImVec2 *points)
//    {
//        int modified = 0;
//        int i;
//        if (maxpoints < 2 || points == 0)
//            return 0;
//
//        if (points[0].x < 0)
//        {
//            points[0].x = 0;
//            points[0].y = 0;
//            points[1].x = 1;
//            points[1].y = 1;
//            points[2].x = -1;
//        }
//
//        ImGuiWindow* window = GetCurrentWindow();
//        ImGuiState& g = *GImGui;
//        const ImGuiStyle& style = g.Style;
//        const ImGuiID id = window->GetID(label);
//        if (window->SkipItems)
//            return 0;
//
//        ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size);
//        ItemSize(bb);
//        if (!ItemAdd(bb, NULL))
//            return 0;
//
//        const bool hovered = IsHovered(bb, id);
//
//        int max = 0;
//        while (max < maxpoints && points[max].x >= 0) max++;
//
//        int kill = 0;
//        do
//        {
//            if (kill)
//            {
//                modified = 1;
//                for (i = kill + 1; i < max; i++)
//                {
//                    points[i - 1] = points[i];
//                }
//                max--;
//                points[max].x = -1;
//                kill = 0;
//            }
//
//            for (i = 1; i < max - 1; i++)
//            {
//                if (abs(points[i].x - points[i - 1].x) < 1 / 128.0)
//                {
//                    kill = i;
//                }
//            }
//        }
//        while (kill);
//
//
//        RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, style.FrameRounding);
//
//        float ht = bb.Max.y - bb.Min.y;
//        float wd = bb.Max.x - bb.Min.x;
//
//        if (hovered)
//        {
//            SetHoveredID(id);
//            if (g.IO.MouseDown[0])
//            {
//                modified = 1;
//                ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
//                pos.y = 1 - pos.y;
//
//                int left = 0;
//                while (left < max && points[left].x < pos.x) left++;
//                if (left) left--;
//
//                ImVec2 p = points[left] - pos;
//                float p1d = sqrt(p.x*p.x + p.y*p.y);
//                p = points[left+1] - pos;
//                float p2d = sqrt(p.x*p.x + p.y*p.y);
//                int sel = -1;
//                if (p1d < (1 / 16.0)) sel = left;
//                if (p2d < (1 / 16.0)) sel = left + 1;
//
//                if (sel != -1)
//                {
//                    points[sel] = pos;
//                }
//                else
//                {
//                    if (max < maxpoints)
//                    {
//                        max++;
//                        for (i = max; i > left; i--)
//                        {
//                            points[i] = points[i - 1];
//                        }
//                        points[left + 1] = pos;
//                    }
//                    if (max < maxpoints)
//                        points[max].x = -1;
//                }
//
//                // snap first/last to min/max
//                if( points[0].x < points[max - 1].x ) {
//                    points[0].x= 0;
//                    points[max - 1].x = 1;
//                } else {
//                    points[0].x= 1;
//                    points[max - 1].x = 0;
//                }
//            }
//        }
//
//        // bg grid
//        window->DrawList->AddLine(
//                ImVec2(bb.Min.x, bb.Min.y + ht / 2),
//                ImVec2(bb.Max.x, bb.Min.y + ht / 2),
//                GetColorU32(ImGuiCol_TextDisabled), 3);
//
//        window->DrawList->AddLine(
//                ImVec2(bb.Min.x, bb.Min.y + ht / 4),
//                ImVec2(bb.Max.x, bb.Min.y + ht / 4),
//                GetColorU32(ImGuiCol_TextDisabled));
//
//        window->DrawList->AddLine(
//                ImVec2(bb.Min.x, bb.Min.y + ht / 4 * 3),
//                ImVec2(bb.Max.x, bb.Min.y + ht / 4 * 3),
//                GetColorU32(ImGuiCol_TextDisabled));
//
//        for (i = 0; i < 9; i++)
//        {
//            window->DrawList->AddLine(
//                    ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Min.y),
//                    ImVec2(bb.Min.x + (wd / 10) * (i + 1), bb.Max.y),
//                    GetColorU32(ImGuiCol_TextDisabled));
//        }
//
//        // smooth curve
//        enum { smoothness = 256 }; // the higher the smoother
//        for( i = 0; i <= (smoothness-1); ++i ) {
//            float px = (i+0) / float(smoothness);
//            float qx = (i+1) / float(smoothness);
//            float py = 1 - CurveValueSmooth(px, maxpoints, points);
//            float qy = 1 - CurveValueSmooth(qx, maxpoints, points);
//            ImVec2 p( px * (bb.Max.x - bb.Min.x) + bb.Min.x, py * (bb.Max.y - bb.Min.y) + bb.Min.y);
//            ImVec2 q( qx * (bb.Max.x - bb.Min.x) + bb.Min.x, qy * (bb.Max.y - bb.Min.y) + bb.Min.y);
//            window->DrawList->AddLine(p, q, GetColorU32(ImGuiCol_PlotLines));
//        }
//
//        // lines
//        for (i = 1; i < max; i++)
//        {
//            ImVec2 a = points[i - 1];
//            ImVec2 b = points[i];
//            a.y = 1 - a.y;
//            b.y = 1 - b.y;
//            a = a * (bb.Max - bb.Min) + bb.Min;
//            b = b * (bb.Max - bb.Min) + bb.Min;
//            window->DrawList->AddLine(a, b, GetColorU32(ImGuiCol_PlotLinesHovered));
//        }
//
//        if (hovered)
//        {
//            // control points
//            for (i = 0; i < max; i++)
//            {
//                ImVec2 p = points[i];
//                p.y = 1 - p.y;
//                p = p * (bb.Max - bb.Min) + bb.Min;
//                ImVec2 a = p - ImVec2(2, 2);
//                ImVec2 b = p + ImVec2(2, 2);
//                window->DrawList->AddRect(a, b, GetColorU32(ImGuiCol_PlotLinesHovered));
//            }
//        }
//
//        // buttons; @todo: mirror, smooth, tessellate
//        if( ImGui::Button("Flip") ) {
//            for( i = 0; i < max; ++i) {
//                points[i].y = 1 - points[i].y;
//            }
//        }
//        ImGui::SameLine();
//
//        // curve selector
//        const char* items[] = {
//                "Custom",
//
//                "Linear",
//                "Quad in",
//                "Quad out",
//                "Quad in  out",
//                "Cubic in",
//                "Cubic out",
//                "Cubic in  out",
//                "Quart in",
//                "Quart out",
//                "Quart in  out",
//                "Quint in",
//                "Quint out",
//                "Quint in  out",
//                "Sine in",
//                "Sine out",
//                "Sine in  out",
//                "Expo in",
//                "Expo out",
//                "Expo in  out",
//                "Circ in",
//                "Circ out",
//                "Circ in  out",
//                "Elastic in",
//                "Elastic out",
//                "Elastic in  out",
//                "Back in",
//                "Back out",
//                "Back in  out",
//                "Bounce in",
//                "Bounce out",
//                "Bounce in out",
//
//                "Sine square",
//                "Exponential",
//
//                "Schubring1",
//                "Schubring2",
//                "Schubring3",
//
//                "SinPi2",
//                "Swing"
//        };
//        static int item = 0;
//        if( modified ) {
//            item = 0;
//        }
//        if( ImGui::Combo("Ease type", &item, items, IM_ARRAYSIZE(items)) ) {
//            max = maxpoints;
//            if( item > 0 ) {
//                for( i = 0; i < max; ++i) {
//                    points[i].x = i / float(max-1);
//                    points[i].y = float( tween::ease( item - 1, points[i].x ) );
//                }
//            }
//        }
//
//        char buf[128];
//        const char *str = label;
//
//        if( hovered ) {
//            ImVec2 pos = (g.IO.MousePos - bb.Min) / (bb.Max - bb.Min);
//            pos.y = 1 - pos.y;
//
//            sprintf(buf, "%s (%f,%f)", label, pos.x, pos.y );
//            str = buf;
//        }
//
//        RenderTextClipped(ImVec2(bb.Min.x, bb.Min.y + style.FramePadding.y), bb.Max, str, NULL, NULL, ImGuiAlign_Center);
//
//        return modified;
//    }
//
//};
//
//
//////#include "bezier.hpp"
//////
//////namespace ImGui {
//////
////////cv::Mat BezierValue(float dt01, float P[]) {
////////    enum { STEPS = 256 };
////////    ImVec2 Q[4] = {{0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1}};
////////    ImVec2 results[STEPS + 1];
////////    bezier_table<STEPS>(Q, results);
////////
////////    cv::Mat mat(1, 256, CV_8UC1);
////////
////////    for (int i = 0; i < 256; ++i) {
////////        mat.at<uchar>(1, i) = cv::saturate_cast<uchar>( results[i].y * 256 );
////////    }
////////    return mat;
////////}
//////
//////
//////
//////int Bezier(const char* label, float P[]) {
//////  // visuals
//////  enum {
//////    SMOOTHNESS = 32
//////  };  // curve smoothness: the higher number of segments, the smoother curve
//////  enum { CURVE_WIDTH = 4 };  // main curved line width
//////  enum { LINE_WIDTH = 1 };   // handlers: small lines width
//////  enum { GRAB_RADIUS = 8 };  // handlers: circle radius
//////  enum { GRAB_BORDER = 2 };  // handlers: circle border width
//////  enum {
//////    AREA_CONSTRAINED = false
//////  };  // should grabbers be constrained to grid area?
//////  enum {
//////    AREA_WIDTH = 220
//////  };  // area width in pixels. 0 for adaptive size (will use max avail width)
//////
//////  // curve presets
//////  static struct {
//////    const char* name;
//////    float points[4];
//////  } presets[] = {
//////      {"Linear", {0.000f, 0.000f, 1.000f, 1.000f}},
//////
//////      {"In Sine", {0.470f, 0.000f, 0.745f, 0.715f}},
//////      {"In Quad", {0.550f, 0.085f, 0.680f, 0.530f}},
//////      {"In Cubic", {0.550f, 0.055f, 0.675f, 0.190f}},
//////      {"In Quart", {0.895f, 0.030f, 0.685f, 0.220f}},
//////      {"In Quint", {0.755f, 0.050f, 0.855f, 0.060f}},
//////      {"In Expo", {0.950f, 0.050f, 0.795f, 0.035f}},
//////      {"In Circ", {0.600f, 0.040f, 0.980f, 0.335f}},
//////      {"In Back", {0.600f, -0.28f, 0.735f, 0.045f}},
//////
//////      {"Out Sine", {0.390f, 0.575f, 0.565f, 1.000f}},
//////      {"Out Quad", {0.250f, 0.460f, 0.450f, 0.940f}},
//////      {"Out Cubic", {0.215f, 0.610f, 0.355f, 1.000f}},
//////      {"Out Quart", {0.165f, 0.840f, 0.440f, 1.000f}},
//////      {"Out Quint", {0.230f, 1.000f, 0.320f, 1.000f}},
//////      {"Out Expo", {0.190f, 1.000f, 0.220f, 1.000f}},
//////      {"Out Circ", {0.075f, 0.820f, 0.165f, 1.000f}},
//////      {"Out Back", {0.175f, 0.885f, 0.320f, 1.275f}},
//////
//////      {"InOut Sine", {0.445f, 0.050f, 0.550f, 0.950f}},
//////      {"InOut Quad", {0.455f, 0.030f, 0.515f, 0.955f}},
//////      {"InOut Cubic", {0.645f, 0.045f, 0.355f, 1.000f}},
//////      {"InOut Quart", {0.770f, 0.000f, 0.175f, 1.000f}},
//////      {"InOut Quint", {0.860f, 0.000f, 0.070f, 1.000f}},
//////      {"InOut Expo", {1.000f, 0.000f, 0.000f, 1.000f}},
//////      {"InOut Circ", {0.785f, 0.135f, 0.150f, 0.860f}},
//////      {"InOut Back", {0.680f, -0.55f, 0.265f, 1.550f}},
//////
//////      // easeInElastic: not a bezier
//////      // easeOutElastic: not a bezier
//////      // easeInOutElastic: not a bezier
//////      // easeInBounce: not a bezier
//////      // easeOutBounce: not a bezier
//////      // easeInOutBounce: not a bezier
//////  };
//////
//////  // preset selector
//////
//////  bool reload = 0;
////////  ImGui::PushID(label);
////////  if (ImGui::ArrowButton(
////////          "##lt",
////////          ImGuiDir_Left)) {  // ImGui::ArrowButton(ImGui::GetCurrentWindow()->GetID("##lt"),
////////                             // ImGuiDir_Left, ImVec2(0, 0), 0)
////////    if (--P[4] >= 0)
////////      reload = 1;
////////    else
////////      ++P[4];
////////  }
////////  ImGui::SameLine();
//////
////////  if (ImGui::Button("Presets")) {
////////    ImGui::OpenPopup("!Presets");
////////  }
////////  if (ImGui::BeginPopup("!Presets")) {
////////    for (int i = 0; i < IM_ARRAYSIZE(presets); ++i) {
////////      if (i == 1 || i == 9 || i == 17)
////////        ImGui::Separator();
////////      if (ImGui::MenuItem(presets[i].name, NULL, P[4] == i)) {
////////        P[4] = i;
////////        reload = 1;
////////      }
////////    }
////////    ImGui::EndPopup();
////////  }
////////  ImGui::SameLine();
////////
////////  if (ImGui::ArrowButton(
////////          "##rt",
////////          ImGuiDir_Right)) {  // ImGui::ArrowButton(ImGui::GetCurrentWindow()->GetID("##rt"),
////////                              // ImGuiDir_Right, ImVec2(0, 0), 0)
////////    if (++P[4] < IM_ARRAYSIZE(presets))
////////      reload = 1;
////////    else
////////      --P[4];
////////  }
////////  ImGui::SameLine();
////////  ImGui::PopID();
//////
////////  if (reload) {
////////    memcpy(P, presets[(int)P[4]].points, sizeof(float) * 4);
////////  }
//////
//////  // bezier widget
//////
//////  const ImGuiStyle& Style = GetStyle();
//////  // const ImGuiIO& IO = GetIO();
//////  ImDrawList* DrawList = GetWindowDrawList();
//////  ImGuiWindow* Window = GetCurrentWindow();
//////  if (Window->SkipItems)
//////    return false;
//////
//////  // header and spacing
//////  int changed = 0;
////////  changed = SliderFloat4(label, P, 0, 1, "%.3f", 1.0f);
//////
//////  int hovered = IsItemActive() || IsItemHovered();  // IsItemDragged() ?
//////  Dummy(ImVec2(0, 3));
//////
//////  // prepare canvas
//////  const float avail = GetContentRegionAvail().x;
//////  const float dim = AREA_WIDTH > 0 ? AREA_WIDTH : avail;
//////  ImVec2 Canvas(dim, dim);
//////
//////  ImRect bb(Window->DC.CursorPos, Window->DC.CursorPos + Canvas);
//////  ItemSize(bb);
//////  if (!ItemAdd(bb, NULL))
//////    return changed;
//////
//////  const ImGuiID id = Window->GetID(label);
//////  hovered |=
//////      0 != ItemHoverable(ImRect(bb.Min, bb.Min + ImVec2(avail, dim)), id);
//////
//////  RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true,
//////              Style.FrameRounding);
//////
//////  // background grid
//////  for (int i = 0; i <= Canvas.x; i += (Canvas.x / 4)) {
//////    DrawList->AddLine(ImVec2(bb.Min.x + i, bb.Min.y),
//////                      ImVec2(bb.Min.x + i, bb.Max.y),
//////                      GetColorU32(ImGuiCol_TextDisabled));
//////  }
//////  for (int i = 0; i <= Canvas.y; i += (Canvas.y / 4)) {
//////    DrawList->AddLine(ImVec2(bb.Min.x, bb.Min.y + i),
//////                      ImVec2(bb.Max.x, bb.Min.y + i),
//////                      GetColorU32(ImGuiCol_TextDisabled));
//////  }
//////
//////  // eval curve
//////  ImVec2 Q[4] = {{0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1}};
//////  ImVec2 results[SMOOTHNESS + 1];
//////  bezier_table<SMOOTHNESS>(Q, results);
//////
//////  // control points: 2 lines and 2 circles
//////  {
//////    // handle grabbers
//////    ImVec2 mouse = GetIO().MousePos, pos[2];
//////    float distance[2];
//////
//////    for (int i = 0; i < 2; ++i) {
//////      pos[i] =
//////          ImVec2(P[i * 2 + 0], 1 - P[i * 2 + 1]) * (bb.Max - bb.Min) + bb.Min;
//////      distance[i] = (pos[i].x - mouse.x) * (pos[i].x - mouse.x) +
//////                    (pos[i].y - mouse.y) * (pos[i].y - mouse.y);
//////    }
//////
//////    int selected = distance[0] < distance[1] ? 0 : 1;
//////    if (distance[selected] < (4 * GRAB_RADIUS * 4 * GRAB_RADIUS)) {
//////      SetTooltip("(%4.3f, %4.3f)", P[selected * 2 + 0], P[selected * 2 + 1]);
//////
//////      if (/*hovered &&*/ (IsMouseClicked(0) || IsMouseDragging(0))) {
//////        float canvasScale = 1.0;
//////        float& px = (P[selected * 2 + 0] +=
//////                     GetIO().MouseDelta.x / (Canvas.x * canvasScale));
//////        float& py = (P[selected * 2 + 1] -=
//////                     GetIO().MouseDelta.y / (Canvas.y * canvasScale));
//////
//////        if (AREA_CONSTRAINED) {
//////          px = (px < 0 ? 0 : (px > 1 ? 1 : px));
//////          py = (py < 0 ? 0 : (py > 1 ? 1 : py));
//////        }
//////
//////        changed = true;
//////      }
//////    }
//////  }
//////
//////  // if (hovered || changed) DrawList->PushClipRectFullScreen();
//////
//////  // draw curve
//////  {
//////    ImColor color(GetStyle().Colors[ImGuiCol_PlotLines]);
//////    for (int i = 0; i < SMOOTHNESS; ++i) {
//////      ImVec2 p = {results[i + 0].x, 1 - results[i + 0].y};
//////      ImVec2 q = {results[i + 1].x, 1 - results[i + 1].y};
//////      ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x,
//////               p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
//////      ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x,
//////               q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
//////      DrawList->AddLine(r, s, color, CURVE_WIDTH);
//////    }
//////  }
//////
//////  // draw preview (cycles every 1s)
//////  ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
////////  static clock_t epoch = clock();
////////  for (int i = 0; i < 3; ++i) {
////////    double now = ((clock() - epoch) / (double)CLOCKS_PER_SEC);
////////    float delta = ((int)(now * 1000) % 1000) / 1000.f;
////////    delta += i / 3.f;
////////    if (delta > 1)
////////      delta -= 1;
////////    int idx = (int)(delta * SMOOTHNESS);
////////    float evalx = results[idx].x;  //
////////    float evaly = results[idx].y;  // ImGui::BezierValue( delta, P );
////////    ImVec2 p0 = ImVec2(evalx, 1 - 0) * (bb.Max - bb.Min) + bb.Min;
////////    ImVec2 p1 = ImVec2(0, 1 - evaly) * (bb.Max - bb.Min) + bb.Min;
////////    ImVec2 p2 = ImVec2(evalx, 1 - evaly) * (bb.Max - bb.Min) + bb.Min;
////////    DrawList->AddCircleFilled(p0, GRAB_RADIUS / 2, ImColor(white));
////////    DrawList->AddCircleFilled(p1, GRAB_RADIUS / 2, ImColor(white));
////////    DrawList->AddCircleFilled(p2, GRAB_RADIUS / 2, ImColor(white));
////////  }
//////
//////  // draw lines and grabbers
//////  float luma = IsItemActive() || IsItemHovered() ? 0.5f : 1.0f;
//////  ImVec4 pink(1.00f, 0.00f, 0.75f, luma), cyan(0.00f, 0.75f, 1.00f, luma);
//////  ImVec2 p1 = ImVec2(P[0], 1 - P[1]) * (bb.Max - bb.Min) + bb.Min;
//////  ImVec2 p2 = ImVec2(P[2], 1 - P[3]) * (bb.Max - bb.Min) + bb.Min;
//////  DrawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), LINE_WIDTH);
//////  DrawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), LINE_WIDTH);
//////  DrawList->AddCircleFilled(p1, GRAB_RADIUS, ImColor(white));
//////  DrawList->AddCircleFilled(p1, GRAB_RADIUS - GRAB_BORDER, ImColor(pink));
//////  DrawList->AddCircleFilled(p2, GRAB_RADIUS, ImColor(white));
//////  DrawList->AddCircleFilled(p2, GRAB_RADIUS - GRAB_BORDER, ImColor(cyan));
//////
//////  // if (hovered || changed) DrawList->PopClipRect();
//////
//////  return changed;
//////}
//////
//////}  // namespace ImGui