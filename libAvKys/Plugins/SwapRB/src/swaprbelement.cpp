/* Webcamoid, webcam capture application.
 * Copyright (C) 2020  Gonzalo Exequiel Pedone
 *
 * Webcamoid is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Webcamoid is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
 *
 * Web-Site: http://webcamoid.github.io/
 */

#include <akfrac.h>
#include <akpacket.h>
#include <akvideocaps.h>
#include <akvideoconverter.h>
#include <akvideopacket.h>

#include "swaprbelement.h"

class SwapRBElementPrivate
{
    public:
        AkVideoConverter m_videoConverter {{AkVideoCaps::Format_rgbap, 0, 0, {}}};
};

SwapRBElement::SwapRBElement(): AkElement()
{
    this->d = new SwapRBElementPrivate;
}

SwapRBElement::~SwapRBElement()
{
    delete this->d;
}

AkPacket SwapRBElement::iVideoStream(const AkVideoPacket &packet)
{
    if (!packet)
        return {};

    auto src = this->d->m_videoConverter.convert(packet);

    if (!src)
        return {};

    AkVideoPacket dst(src);
    dst.copyMetadata(src);

    for (int y = 0; y < src.caps().height(); ++y) {
        auto srcLineR = src.constLine(0, y);
        auto srcLineG = src.constLine(1, y);
        auto srcLineB = src.constLine(2, y);
        auto srcLineA = src.constLine(3, y);

        auto dstLineR = dst.line(0, y);
        auto dstLineG = dst.line(1, y);
        auto dstLineB = dst.line(2, y);
        auto dstLineA = dst.line(3, y);

        for (int x = 0; x < src.caps().width(); ++x) {
            dstLineR[x] = srcLineB[x];
            dstLineG[x] = srcLineG[x];
            dstLineB[x] = srcLineR[x];
            dstLineA[x] = srcLineA[x];
        }
    }

    if (dst)
        emit this->oStream(dst);

    return dst;
}

#include "moc_swaprbelement.cpp"
