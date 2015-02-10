#include "ast_stringify.h"
using namespace ring::ringc;
using namespace ring::ringc::ast;


static string c_Red("\033[1;31m");
static string c_Cyan("\033[1;36m");
static string c_Green("\033[1;32m");
static string c_green("\033[0;32m");
static string c_Blue("\033[1;34m");
static string c_Black("\033[1;30m");
static string c_Brown("\033[1;33m");
static string c_Magenta("\033[1;35m");
static string c_magenta("\033[0;35m");
static string c_Gray("\033[1;37m");
static string c_gray("\033[0;37m");
static string c_none("\033[0m");


string intToString(int v) {
	char buf[15] = {0};
	sprintf(buf, "%d", v);
	return buf;
}


AstStringify::AstStringify(Session* session)
		: _session(session) {
}


string AstStringify::toString(AssignmentOp op) {
	switch (op) {
		case AO_ASSIGN: return "=";
		case AO_ADD: return "+=";
		case AO_SUB: return "-=";
		case AO_MUL: return "*=";
		case AO_DIV: return "/=";
		case AO_MOD: return "%=";
		case AO_LSH: return "<<=";
		case AO_RSH: return ">>=";
		case AO_BITWISE_OR: return "|=";
		case AO_BITWISE_XOR: return "^=";
		case AO_BITWISE_AND: return "&=";
	}
	return "Unkown assignment operator";
}


string AstStringify::toString(LogicalOp op) {
	switch (op) {
		case LO_OR: return "||";
		case LO_AND: return "&&";
	}
	return "Unkown logical operator";
}


string AstStringify::toString(BinaryOp op) {
	switch (op) {
		case BO_ADD: return "+";
		case BO_SUB: return "-";
		case BO_MUL: return "*";
		case BO_DIV: return "/";
		case BO_MOD: return "%";
		case BO_EXP: return "**";
		case BO_BITWISE_OR: return "|";
		case BO_BITWISE_XOR: return "^";
		case BO_BITWISE_AND: return "&";
		case BO_LSH: return "<<";
		case BO_RSH: return ">>";
		case BO_EQ: return "==";
		case BO_NE: return "!=";
		case BO_LT: return "<";
		case BO_LE: return "<=";
		case BO_GE: return ">=";
		case BO_GT: return ">";
	}
	return "Unkown binary operator";
}


string AstStringify::toString(UnaryOp op) {
	switch (op) {
		case UO_PLUS: return "+";
		case UO_MINUS: return "-";
		case UO_NOT: return "!";
		case UO_BITWISE_NOT: return "~";
	}
	return "Unkown unary operator";
}


string AstStringify::toString(PrimitiveTypeType prim_type) {
	switch (prim_type) {
		case PRIM_TYPE_NIL: return "nil";
		case PRIM_TYPE_INT: return "int";
		case PRIM_TYPE_BOOL: return "bool";
	}
	return "Unknown primitive type";
}


string AstStringify::toString(TypeId type_id) {
	if (type_id.some()) {
		Type* type = session()->type_table()->getType(type_id);
		if (type) {
			return toString(type);
		}
	}
	return "Unknown type";
}


string AstStringify::toString(Type* type) {
	switch (type->type()) {
		case TYPE_PRIMITIVE: return toString(static_cast<PrimitiveType*>(type));
		case TYPE_FUNCTION:  return toString(static_cast<FunctionType*>(type));
		case TYPE_ARRAY: return toString(static_cast<ArrayType*>(type));
	}
	return "Unknown type";
}


string AstStringify::toString(PrimitiveType* type) {
	return toString(type->primitive_type());
}


string AstStringify::toString(FunctionType* type) {
	string res = "";
	res += "(";
	bool first = true;
	for_each (type->args().begin(), type->args().end(), [this, &res, &first, type] (TypeId type_id) {
		if (!first) res += ", ";
		res += toString(type_id);
		first = false;
	});
	res += ") -> ";
	res += toString(type->ret());
	return res;
}


string AstStringify::toString(ArrayType* type) {
	string res = "";
	res += "[] ";
	res += toString(type->elem_type());
	return res;
}


string AstStringify::toString(Module* module) {
	return toStringAst(module, "Module");
}


string AstStringify::toString(Use* use) {
	return toStringAst(use, "Use");
}


string AstStringify::toString(Extern* ext) {
	string body =  "Extern ";
	body += c_Magenta + session()->str(ext->name().name_id()) + c_none + " ";
	body += c_Green + "(" + intToString(ext->name().symbol_id().value()) + ")" + c_none;
	body += c_gray + " : " + toString(ext->type_id()) + c_none;
	return toStringAst(ext, body);
}


string AstStringify::toString(Stmt* stmt) {
	STMT_DISPATCH(toString, stmt);
	return "Unknown statement";
}


string AstStringify::toString(Let* let) {
	string body = "Let ";
	body += let->is_pub() ? "pub " : "";
	body += let->is_mut() ? "mut " : "";
	body += c_Magenta + session()->str(let->name().name_id()) + c_none + " ";
	body += c_Green + "(" + intToString(let->name().symbol_id().value()) + ")" + c_none;
	body += c_gray + " : " + toString(let->type_id()) + c_none;
	return toStringAst(let, body);
}


string AstStringify::toString(Expr* expr) {
	EXPR_DISPATCH(toString, expr);
	return "Unknown expression";
}


string AstStringify::toString(ExprEmpty* empty) {
	return toStringExpr(empty, "ExprEmpty");
}


string AstStringify::toString(ExprFn* fn) {
	return toStringExpr(fn, "ExprFn");
}


string AstStringify::toString(ExprBlock* block) {
	return toStringExpr(block, "ExprBlock");
}


string AstStringify::toString(ExprIf* if_) {
	return toStringExpr(if_, "ExprIf");
}


string AstStringify::toString(ExprIdent* ident) {
	string body = "ExprIdent ";
	body += c_magenta + session()->str(ident->id().name_id()) + c_none;
	body += " " + c_green + "(" + intToString(ident->id().symbol_id().value()) + ")" + c_none;
	return toStringExpr(ident, body);
}


string AstStringify::toString(ExprLiteral* lit) {
	string body = "ExprLiteral ";
	if (lit->lit_type() == LIT_BASIC) {
		ExprLiteralBasic* lit_basic = static_cast<ExprLiteralBasic*>(lit);
		body += c_magenta + session()->str(lit_basic->str_id()) + c_none;
	} else if (lit->lit_type() == LIT_ARRAY) {
		body += c_magenta + "Array" + c_none;
	}
	return toStringExpr(lit, body);
}


string AstStringify::toString(ExprMember* member) {
	return toStringExpr(member, "ExprMember");
}


string AstStringify::toString(ExprCall* call) {
	return toStringExpr(call, "ExprCall");
}


string AstStringify::toString(ExprUnary* unary) {
	string body = "ExprUnary ";
	body += "( ";
	body += toString(unary->op());
	body += " )";
	return toStringExpr(unary, body);
}


string AstStringify::toString(ExprBinary* binary) {
	string body = "ExprBinary ";
	body += "( ";
	body += toString(binary->op());
	body += " )";
	return toStringExpr(binary, "ExprBinary");
}


string AstStringify::toString(ExprLogical* logical) {
	string body = "ExprLogical ";
	body += "( ";
	body += toString(logical->op());
	body += " )";
	return toStringExpr(logical, "ExprLogical");
}


string AstStringify::toString(ExprConditional* cond) {
	return toStringExpr(cond, "ExprConditional");
}


string AstStringify::toString(ExprAssignment* assign) {
	string body = "ExprAssignment ";
	body += "( ";
	body += toString(assign->op());
	body += " )";
	return toStringExpr(assign, "ExprAssignment");
}



string AstStringify::toStringAst(AstNode* node, const string& body) {
	string res = c_Cyan + "[" + intToString(node->node_id().value()) + "] " + c_none;
	res += body;
	return res;
}


string AstStringify::toStringExpr(Expr* expr, const string& body) {
	string res = toStringAst(expr, body);
	res += c_gray + " : " + toString(expr->type_id()) + c_none;
	return res;
}

