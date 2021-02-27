#include "common/common.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "absl/strings/escaping.h"

using namespace std;

struct NameDef {
    int id;
    string srcName;
    string val;
    bool isConstant;

    NameDef(string_view srcName, string_view val, bool isConstant)
        : srcName(string(srcName)), val(string(val)), isConstant(isConstant) {}
    NameDef(string_view srcName, string_view val) : srcName(string(srcName)), val(string(val)), isConstant(false) {
        if (srcName == val) {
            throw std::logic_error("Only pass one arg for '" + string(val) + "'");
        }
    }
    NameDef(string_view srcName) : srcName(string(srcName)), val(string(srcName)), isConstant(false){};
};

NameDef names[] = {
    {"initialize"},
    {"andAnd", "&&"},
    {"orOr", "||"},
    {"toS", "to_s"},
    {"toA", "to_a"},
    {"toAry", "to_ary"},
    {"toH", "to_h"},
    {"toHash", "to_hash"},
    {"toProc", "to_proc"},
    {"concat"},
    {"key_p", "key?"},
    {"intern"},
    {"call"},
    {"bang", "!"},
    {"squareBrackets", "[]"},
    {"squareBracketsEq", "[]="},
    {"unaryPlus", "+@"},
    {"unaryMinus", "-@"},
    {"star", "*"},
    {"starStar", "**"},
    {"ampersand", "&"},
    {"tripleEq", "==="},
    {"orOp", "|"},
    {"backtick", "`"},
    {"slice"},
    {"defined_p", "defined?"},
    {"undef"},
    {"each"},

    // used in CFG for temporaries
    {"whileTemp", "<whileTemp>"},
    {"ifTemp", "<ifTemp>"},
    {"returnTemp", "<returnTemp>"},
    {"statTemp", "<statTemp>"},
    {"assignTemp", "<assignTemp>"},
    {"returnMethodTemp", "<returnMethodTemp>"},
    {"debugEnvironmentTemp", "<debugEnvironmentTemp>"},
    {"blockReturnTemp", "<blockReturnTemp>"},
    {"nextTemp", "<nextTemp>"},
    {"selfMethodTemp", "<selfMethodTemp>"},
    {"selfLocal", "<self>"},
    {"selfRestore", "<selfRestore>"}, // used to restore self in case block changes it
    {"hashTemp", "<hashTemp>"},
    {"arrayTemp", "<arrayTemp>"},
    {"rescueTemp", "<rescueTemp>"},
    {"exceptionValue", "<exceptionValue>"},
    {"gotoDeadTemp", "<gotoDeadTemp>"},
    {"exceptionClassTemp", "<exceptionClassTemp>"},
    {"isaCheckTemp", "<isaCheckTemp>"},
    {"keepForCfgTemp", "<keepForCfgTemp>"},
    {"retryTemp", "<retryTemp>"},
    {"throwAwayTemp", "<throwAwayTemp>"},
    {"castTemp", "<castTemp>"},
    {"finalReturn", "<finalReturn>"},
    {"cfgAlias", "<cfgAlias>"},
    {"magic", "<magic>"},
    {"unconditional", "<unconditional>"},
    {"argPresent", "<argPresent>"},
    // end CFG temporaries

    {"include"},
    {"extend"},
    {"currentFile", "__FILE__"},
    {"merge"},

    // T keywords
    {"sig"},
    {"typeParameters", "type_parameters"},
    {"typeParameter", "type_parameter"},
    {"abstract"},
    {"implementation"},
    {"override_", "override"},
    {"overridable"},
    {"allowIncompatible", "allow_incompatible"},

    // Sig builders
    {"bind"},
    {"params"},
    {"final_", "final"},
    {"returns"},
    {"void_", "void"},
    {"VOID", "VOID", true},
    {"checked"},
    {"never"},
    {"onFailure", "on_failure"},

    {"all"},
    {"any"},
    {"enum_", "enum"},
    {"enums"},
    {"nilable"},
    {"proc"},
    {"untyped"},
    {"noreturn"},
    {"singletonClass", "singleton_class"},
    {"class_", "class"},
    {"classOf", "class_of"},
    {"selfType", "self_type"},
    {"experimentalAttachedClass", "experimental_attached_class"},
    {"attachedClass", "attached_class"},
    {"coerce"},

    {"assertType", "assert_type!"},
    {"cast"},
    {"let"},
    {"uncheckedLet", "<unchecked_let>"},
    {"unsafe"},
    {"must"},
    {"declareInterface", "interface!"},
    {"declareAbstract", "abstract!"},
    {"declareFinal", "final!"},
    {"declareSealed", "sealed!"},
    {"revealType", "reveal_type"},
    {"absurd"},
    {"nonForcingIsA_p", "non_forcing_is_a?"},
    // end T keywords

    // Ruby DSL methods which we understand
    {"attr"},
    {"attrAccessor", "attr_accessor"},
    {"attrWriter", "attr_writer"},
    {"attrReader", "attr_reader"},
    {"private_", "private"},
    {"protected_", "protected"},
    {"public_", "public"},
    {"privateClassMethod", "private_class_method"},
    {"privateConstant", "private_constant"},
    {"moduleFunction", "module_function"},
    {"aliasMethod", "alias_method"},

    {"flatfile"},
    {"from"},
    {"field"},
    {"pattern"},

    // type alias names
    {"typeAlias", "type_alias"},
    {"typeMember", "type_member"},
    {"typeTemplate", "type_template"},
    {"covariant", "out"},
    {"contravariant", "in"},
    {"invariant", "<invariant>"},
    {"fixed"},
    {"lower"},
    {"upper"},

    {"prop"},
    {"tokenProp", "token_prop"},
    {"timestampedTokenProp", "timestamped_token_prop"},
    {"createdProp", "created_prop"},
    {"merchantProp", "merchant_prop"},
    {"encryptedProp", "encrypted_prop"},
    {"array"},
    {"defDelegator", "def_delegator"},
    {"delegate"},
    {"type"},
    {"optional"},
    {"immutable"},
    {"migrate"},
    {"default_", "default"},
    {"const_", "const"},
    {"token"},
    {"created"},
    {"merchant"},
    {"foreign"},
    {"ifunset"},
    {"withoutAccessors", "without_accessors"},
    {"instanceVariableGet", "instance_variable_get"},
    {"instanceVariableSet", "instance_variable_set"},
    {"decorator"},
    {"propGetLogic", "prop_get_logic"},
    {"softFreezeLogic", "soft_freeze_logic"},
    {"computedBy", "computed_by"},
    {"factory"},
    {"InexactStruct", "InexactStruct", true},
    {"Chalk", "Chalk", true},
    {"ODM", "ODM", true},
    {"Document", "Document", true},
    {"Private", "Private", true},
    {"Types", "Types", true},
    {"DocumentDecoratorHelper", "DocumentDecoratorHelper", true},
    {"Chalk_ODM_Document", "::Chalk::ODM::Document"},

    {"prefix"},
    {"to"},

    {"mattrAccessor", "mattr_accessor"},
    {"mattrReader", "mattr_reader"},
    {"mattrWriter", "mattr_writer"},
    {"cattrAccessor", "cattr_accessor"},
    {"cattrReader", "cattr_reader"},
    {"cattrWriter", "cattr_writer"},
    {"instanceReader", "instance_reader"},
    {"instanceWriter", "instance_writer"},
    {"instanceAccessor", "instance_accessor"},
    {"instancePredicate", "instance_predicate"},
    {"classAttribute", "class_attribute"},

    {"describe"},
    {"it"},
    {"before"},
    {"after"},
    {"afterAngles", "<after>"},
    {"testEach", "test_each"},
    {"testEachHash", "test_each_hash"},
    {"constSet", "const_set"},
    {"collect"},

    {"dslOptional", "dsl_optional"},
    {"dslRequired", "dsl_required"},
    {"implied"},
    {"skipGetter", "skip_getter"},
    {"skipSetter", "skip_setter"},

    {"wrapInstance", "wrap_instance"},

    {"registered"},
    {"instanceRegistered", "<instance_registered>"},
    {"helpers"},

    {"keywordInit", "keyword_init"},

    {"DB", "DB", true},
    {"Model", "Model", true},
    {"Mixins", "Mixins", true},
    {"Encryptable", "Encryptable", true},
    {"EncryptedValue", "EncryptedValue", true},
    {"Command", "Command", true},
    {"Enum", "Enum", true},

    {"ActiveRecord", "ActiveRecord", true},
    {"Migration", "Migration", true},
    {"Compatibility", "Compatibility", true},
    {"ActiveSupport", "ActiveSupport", true},
    {"Concern", "Concern", true},

    {"instance"},
    {"normal"},

    {"raise"},
    {"rewriterRaiseUnimplemented", "Sorbet rewriter pass partially unimplemented"},

    {"test"},
    // end DSL methods

    // The next two names are used as keys in SymbolInfo::members to store
    // pointers up and down the singleton-class hierarchy. If A's singleton
    // class is B, then A will have a `singletonClass` entry in its members
    // table which references B, and B will have an `attachedClass` entry
    // pointing at A.
    //
    // The "attached class" terminology is borrowed from MRI, which refers
    // to the unique instance attached to a singleton class as the "attached
    // object"
    {"singleton", "<singleton class>"},
    {"attached", "<attached class>"},

    // Requires ancestor
    {"requiredAncestors", "<required-ancestor>"},
    {"requiredAncestorsLin", "<required-ancestor-lin>"},
    {"requiresAncestor", "requires_ancestor"},

    // This behaves like the above two names, in the sense that we use a member
    // on a class to lookup an associated symbol with some extra info.
    {"sealedSubclasses", "sealed_subclasses"},

    // Used to store arguments to a "mixes_in_class_methods()" call
    {"mixedInClassMethods", "<mixed_in_class_methods>"},
    {"mixesInClassMethods", "mixes_in_class_methods"},
    {"ClassMethods", "ClassMethods", true},
    {"classMethods", "class_methods"},

    {"blockTemp", "<block>"},
    {"blockRetrunType", "<block-return-type>"},
    {"blockPreCallTemp", "<block-pre-call-temp>"},
    {"blockPassTemp", "<block-pass>"},
    {"forTemp"},
    {"new_", "new"},
    {"blockCall", "<block-call>"},
    {"blockBreakAssign", "<block-break-assign>"},
    {"arg", "<arg>"},
    {"blkArg", "<blk>"},
    {"blockGiven_p", "block_given?"},

    // Used to generate temporary names for destructuring arguments ala proc do
    //  |(x,y)|; end
    {"destructureArg", "<destructure>"},

    {"lambda"},
    {"nil_p", "nil?"},
    {"blank_p", "blank?"},
    {"present_p", "present?"},
    {"nil"},
    {"super"},
    {"empty", ""},

    {"buildHash", "<build-hash>"},
    {"buildKeywordArgs", "<build-keyword-args>"},
    {"buildArray", "<build-array>"},
    {"buildRange", "<build-range>"},
    {"splat", "<splat>"},
    {"expandSplat", "<expand-splat>"},
    {"suggestType", "<suggest-type>"},
    {"checkMatchArray", "<check-match-array>"},
    {"arg0"},
    {"arg1"},
    {"arg2"},
    {"opts"},
    {"args"},
    {"Elem", "Elem", true},
    {"keepForIde", "keep_for_ide"},
    {"keepForTypechecking", "keep_for_typechecking"},
    {"keepDef", "keep_def"},
    {"keepSelfDef", "keep_self_def"},
    {"keepForCfg", "<keep-for-cfg>"},
    {"retry", "<retry>"},
    {"unresolvedAncestors", "<unresolved-ancestors>"},
    {"defineTopClassOrModule", "<define-top-class-or-module>"},
    {"nilForSafeNavigation", "<nil-for-safe-navigation>"},

    {"isA_p", "is_a?"},
    {"kindOf_p", "kind_of?"},
    {"lessThan", "<"},
    {"equal_p", "equal?"},
    {"eqeq", "=="},
    {"neq", "!="},
    {"leq", "<="},
    {"geq", ">="},

    // methods that are known by tuple and\or shape types
    {"freeze"},
    {"last"},
    {"first"},
    {"min"},
    {"max"},

    // Argument forwarding
    {"fwdArgs", "<fwd-args>"},
    {"fwdKwargs", "<fwd-kwargs>"},
    {"fwdBlock", "<fwd-block>"},

    // Enumerable#flat_map has special-case logic in Infer
    {"flatMap", "flat_map"},

    // Array#flatten, #product, #compact and #zip are also custom-implemented
    {"flatten"},
    {"product"},
    {"compact"},
    {"zip"},

    // Pattern matching
    {"patternMatch", "<pattern-match>"},

    {"staticInit", "<static-init>"},

    {"require"},
    {"callWithSplat", "<call-with-splat>"},
    {"callWithBlock", "<call-with-block>"},
    {"callWithSplatAndBlock", "<call-with-splat-and-block>"},
    {"enumerableToH", "enumerable_to_h"},
    {"blockBreak", "<block-break>"},
    {"selfNew", "<self-new>"},
    {"stringInterpolate", "<string-interpolate>"},

    // Packager
    {"import"},
    {"export_", "export"},
    {"PackageSpec", "PackageSpec", true},
    {"PackageRegistry", "<PackageRegistry>", true},
    {"exportMethods", "export_methods"},
    {"PackageMethods", "<PackageMethods>", true},
    {"PkgRoot_Package", "PkgRoot_Package", true},

    // GlobalState initEmpty()
    {"Top", "<top>", true},
    {"Bottom", "T.noreturn", true},
    {"Untyped", "T.untyped", true},
    {"Root", "<root>", true},
    {"Object", "Object", true},
    {"String", "String", true},
    {"Integer", "Integer", true},
    {"Float", "Float", true},
    {"Symbol", "Symbol", true},
    {"Array", "Array", true},
    {"Hash", "Hash", true},
    {"Proc", "Proc", true},
    {"TrueClass", "TrueClass", true},
    {"FalseClass", "FalseClass", true},
    {"Boolean", "Boolean", true},
    {"NilClass", "NilClass", true},
    {"Class", "Class", true},
    {"Module", "Module", true},
    {"Todo", "<todo sym>", true},
    {"TodoMethod", "<todo method>", false},
    {"NoSymbol", "<none>", true},
    {"noFieldOrStaticField", "<no-field-or-static-field>", false},
    {"noMethod", "<no-method>", false},
    {"NoTypeArgument", "<no-type-argument>", true},
    {"NoTypeMember", "<no-type-member>", true},
    {"Opus", "Opus", true},
    {"T", "T", true},
    {"BasicObject", "BasicObject", true},
    {"Kernel", "Kernel", true},
    {"Range", "Range", true},
    {"Regexp", "Regexp", true},
    {"StandardError", "StandardError", true},
    {"Complex", "Complex", true},
    {"Rational", "Rational", true},
    // A magic non user-creatable class with methods to keep state between passes
    {"Magic", "<Magic>", true},
    // A magic non user-creatable class for mimicing the decl builder during cfg
    // construction
    {"DeclBuilderForProcs", "<DeclBuilderForProcs>", true},
    {"Enumerable", "Enumerable", true},
    {"Enumerator", "Enumerator", true},
    {"Lazy", "Lazy", true},
    {"Set", "Set", true},
    {"Struct", "Struct", true},
    {"File", "File", true},
    {"Encoding", "Encoding", true},
    {"getEncoding", "<get-encoding>"},
    {"Static", "Static", true},
    {"StubModule", "StubModule", true},
    {"StubSuperClass", "StubSuperClass", true},
    {"StubMixin", "StubMixin", true},
    {"Base", "Base", true},
    {"Void", "Void", true},
    {"TypeAlias", "<TypeAlias>", true},
    {"Tools", "Tools", true},
    {"Accessible", "Accessible", true},
    {"Generic", "Generic", true},
    {"Tuple", "Tuple", true},
    {"Shape", "Shape", true},
    {"Subclasses", "SUBCLASSES", true},
    {"Sorbet", "Sorbet", true},
    {"ReturnTypeInference", "ReturnTypeInference", true},
    {"InferredReturnType", "INFERRED_RETURN_TYPE", true},
    {"InferredArgumentType", "INFERRED_ARGUMENT_TYPE", true},
    {"ImplicitModuleSuperclass", "ImplicitModuleSuperclass", true},
    {"guessedTypeTypeParameterHolder", "guessed_type_type_parameter_holder"},
    {"Builder", "Builder", true},
    {"Sig", "Sig", true},
    {"Utils", "Utils", true},
    {"UndeclaredFieldStub", "<undeclared-field-stub>", true},
    {"badAliasMethodStub", "<bad-method-alias-stub>"},
    {"Helpers", "Helpers", true},
    {"Net", "Net", true},
    {"IMAP", "IMAP", true},
    {"Protocol", "Protocol", true},
    {"WithoutRuntime", "WithoutRuntime", true},
    {"Singleton", "Singleton", true},
    {"AttachedClass", "<AttachedClass>", true},
    {"NonForcingConstants", "NonForcingConstants", true},
    {"VERSION", "VERSION", true},
};

void emit_name_header(ostream &out, NameDef &name) {
    out << "#ifndef NAME_" << name.srcName << '\n';
    out << "#define NAME_" << name.srcName << '\n';
    out << "    // \"" << name.val << "\"" << '\n';
    out << "    static inline constexpr NameRef " << name.srcName << "() {" << '\n';
    out << "        return NameRef(NameRef::WellKnown{}, NameKind::" << (name.isConstant ? "CONSTANT" : "UTF8") << ", "
        << name.id << ");" << '\n';
    out << "    }" << '\n';
    out << "#endif" << '\n';
    out << '\n';
}

void emit_name_string(ostream &out, NameDef &name) {
    out << "const char *" << name.srcName << " = \"";
    out << absl::CEscape(name.val) << "\";" << '\n';

    out << "std::string_view " << name.srcName << "_DESC{(char*)";
    out << name.srcName << "," << name.val.size() << "};" << '\n';
    out << '\n';
}

void emit_register(ostream &out) {
    out << "void registerNames(GlobalState &gs) {" << '\n';
    for (auto &name : names) {
        auto fun = name.isConstant ? "enterNameConstant" : "enterNameUTF8";
        out << "    NameRef " << name.srcName << "_id = gs." << fun << "(" << name.srcName << "_DESC);" << '\n';
    }
    out << '\n';
    for (auto &name : names) {
        out << "    ENFORCE(" << name.srcName << "_id." << (name.isConstant ? "constantIndex" : "utf8Index")
            << "() == " << name.id << "); /* " << name.srcName << "() */" << '\n';
    }
    out << '\n';
    out << "}" << '\n';
}

int main(int argc, char **argv) {
    int constantI = 0;
    int utf8I = 0;
    for (auto &name : names) {
        if (name.isConstant) {
            utf8I++;
            name.id = constantI++;
        } else {
            name.id = utf8I++;
        }
    }
    int lastConstantId = constantI;
    int lastUtf8Id = utf8I;

    // emit header file
    {
        ofstream header(argv[1], ios::trunc);
        if (!header.good()) {
            cerr << "unable to open " << argv[1] << '\n';
            return 1;
        }
        header << "#include \"core/NameRef.h\"" << '\n' << '\n';
        header << "namespace sorbet {" << '\n';
        header << "namespace core {" << '\n';
        header << "class GlobalState;" << '\n';
        header << "namespace Names {" << '\n';

        for (auto &name : names) {
            if (!name.isConstant) {
                emit_name_header(header, name);
            }
        }

        header << "namespace Constants {" << '\n';
        for (auto &name : names) {
            if (name.isConstant) {
                emit_name_header(header, name);
            }
        }
        header << "}" << '\n';

        header << "#ifndef NAME_LAST_WELL_KNOWN_CONSTANT_NAME" << '\n';
        header << "#define NAME_LAST_WELL_KNOWN_CONSTANT_NAME" << '\n';
        header << "constexpr int LAST_WELL_KNOWN_CONSTANT_NAME = " << lastConstantId << ";" << '\n';
        header << "#endif" << '\n';

        header << "#ifndef NAME_LAST_WELL_KNOWN_UTF8_NAME" << '\n';
        header << "#define NAME_LAST_WELL_KNOWN_UTF8_NAME" << '\n';
        header << "constexpr int LAST_WELL_KNOWN_UTF8_NAME = " << lastUtf8Id << ";" << '\n';
        header << "#endif" << '\n';

        header << "    void registerNames(GlobalState &gs);" << '\n';
        header << "}" << '\n';
        header << "}" << '\n';
        header << "}" << '\n';
    }

    // emit initialization .cc file
    {
        ofstream classfile(argv[2], ios::trunc);
        if (!classfile.good()) {
            cerr << "unable to open " << argv[2] << '\n';
            return 1;
        }
        classfile << "#include \"core/GlobalState.h\"" << '\n' << '\n';
        classfile << "#include \"core/Names.h\"" << '\n' << '\n';
        classfile << "#include \"core/Names_gen.h\"" << '\n' << '\n';
        classfile << "namespace sorbet {" << '\n';
        classfile << "namespace core {" << '\n';
        classfile << "namespace Names {" << '\n';
        classfile << "namespace {" << '\n';
        for (auto &name : names) {
            emit_name_string(classfile, name);
        }
        classfile << "}" << '\n';
        classfile << '\n';

        emit_register(classfile);

        classfile << "}" << '\n';
        classfile << "}" << '\n';
        classfile << "}" << '\n';
    }

    return 0;
}
