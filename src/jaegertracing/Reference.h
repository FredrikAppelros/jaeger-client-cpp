/*
 * Copyright (c) 2017 Uber Technologies, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef JAEGERTRACING_REFERENCE_H
#define JAEGERTRACING_REFERENCE_H

#include <sstream>
#include <string>

#include <opentracing/propagation.h>

#include "jaegertracing/Compilers.h"
#include "jaegertracing/SpanContext.h"
#include "jaegertracing/thrift-gen/jaeger_types.h"

namespace jaegertracing {

class Reference {
  public:
    using Type = opentracing::SpanReferenceType;

    Reference(const SpanContext& spanContext, Type type)
        : _spanContext(spanContext)
        , _type(type)
    {
    }

    const SpanContext& spanContext() const { return _spanContext; }

    Type type() const { return _type; }

    thrift::SpanRef thrift() const
    {
        thrift::SpanRef spanRef;
        switch (_type) {
        case Type::ChildOfRef: {
            spanRef.__set_refType(thrift::SpanRefType::CHILD_OF);
        } break;
        case Type::FollowsFromRef: {
            spanRef.__set_refType(thrift::SpanRefType::FOLLOWS_FROM);
        } break;
        default: {
            std::ostringstream oss;
            oss << "Invalid span reference type " << static_cast<int>(_type)
                << ", context " << _spanContext;
            throw std::invalid_argument(oss.str());
        } break;
        }
        spanRef.__set_traceIdHigh(_spanContext.traceID().high());
        spanRef.__set_traceIdLow(_spanContext.traceID().low());
        spanRef.__set_spanId(_spanContext.spanID());
        return spanRef;
    }

  private:
    SpanContext _spanContext;
    Type _type;
};

}  // namespace jaegertracing

#endif  // JAEGERTRACING_REFERENCE_H
