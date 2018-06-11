#ifndef SORBET_DSL_DSL_H
#define SORBET_DSL_DSL_H
#include "ast/ast.h"
#include <memory>

namespace sorbet {
namespace dsl {

class DSL final {
public:
    static std::unique_ptr<ast::Expression> run(core::MutableContext ctx, std::unique_ptr<ast::Expression> tree);

    DSL() = delete;
};

} // namespace dsl
} // namespace sorbet

#endif
