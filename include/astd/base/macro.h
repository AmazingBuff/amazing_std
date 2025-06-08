#pragma once

#define NAMESPACE_BEGIN(name)   namespace name {
#define NAMESPACE_END(name)     }

#define AMAZING_NAMESPACE_BEGIN NAMESPACE_BEGIN(Amazing)
#define AMAZING_NAMESPACE_END   NAMESPACE_END(Amazing)

#define INTERNAL_NAMESPACE_BEGIN  NAMESPACE_BEGIN(Internal)
#define INTERNAL_NAMESPACE_END  NAMESPACE_END(Internal)

#define NODISCARD [[nodiscard]]