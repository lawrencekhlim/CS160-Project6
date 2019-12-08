#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.




void CodeGenerator::visitProgramNode(ProgramNode* node) {
	// WRITEME: Replace with code if necessary
	std::cout << ".data" << std::endl;
	std::cout << "printstr: .asciz \"%d\\n\"" << std::endl;
	std::cout << ".text" << std::endl;
	std::cout << ".globl Main_main" << std::endl;
	node->visit_children(this);
	
}

void CodeGenerator::visitClassNode(ClassNode* node) {
	// WRITEME: Replace with code if necessary
	currentClassName = node->identifier_1->name;

	// Only do declarations if class is main
	node->visit_children(this);
}

void CodeGenerator::visitMethodNode(MethodNode* node) {
	currentMethodName = node->identifier->name;
	std::cout << currentClassName << "_" << currentMethodName << ": " << std::endl;
	std::cout << "  push %ebp" << std::endl;
	std::cout << "  mov %esp, %ebp" << std::endl;
	int offset = 4*(*(*classTable)[currentClassName].methods)[currentMethodName].localsSize;
	std::cout << "  sub $"<< offset  <<", %esp" << std::endl;

	std::cout << "  push %ebx" << std::endl;
	std::cout << "  push %esi" << std::endl;
	std::cout << "  push %edi" << std::endl;

	node->visit_children (this);

	std::cout << "  pop %edi" << std::endl;
	std::cout << "  pop %esi" << std::endl;
	std::cout << "  pop %ebx" << std::endl;
	
	std::cout << "  add $" << offset << ", %esp"<< std::endl;
	std::cout << "  mov %ebp, %esp" << std::endl;
	std::cout << "  pop %ebp" << std::endl;
	std::cout << "  ret" << std::endl;
}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
	node->visit_children(this);
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {
	// Nothing to do here?
}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {
	// ClassNode - No need to handle since malloc already handles it
	// MethodNode - Handled inside MethodNode
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
	std::cout << "  # Return Statement" << std::endl;
	node->visit_children(this);
	std::cout << "  pop %eax" << std::endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
	// Several cases: 
	// Local variable
	// Class variable
	// Superclass variables
	node->visit_children (this);
	if (node->identifier_2 == NULL) {
		// Local variable
		// Find variable on stack first
		auto localTable = (*(*(*classTable)[currentClassName].methods)[currentMethodName].variables);
		if (localTable.find(node->identifier_1->name) != localTable.end()) {
			int offset = localTable[node->identifier_1->name].offset;
			std::cout << "  mov 0(%esp), %eax" << std::endl;
			std::cout << "  mov %eax, " << offset << "(%ebp)" << std::endl;
			std::cout << "  add $4, %esp" << std::endl;
		}
		else {
			int offset = (*(*classTable)[currentClassName].members)[node->identifier_2->name].offset;
			std::cout << "  mov 0(%esp), %eax"  << std::endl;
			std::cout << "  mov %eax, " << offset << "(%ebp)" << std::endl;
			std::cout << "  add $4, %esp" << std::endl;
		}
	}
	else {

		auto className = (*(*classTable)[currentClassName].members)[node->identifier_1->name].type.objectClassName;
		int memberOffset = (*(*classTable)[currentClassName].members)[node->identifier_1->name].offset;
		
		int offset = (*(*classTable)[className].members)[node->identifier_2->name].offset;
		auto localVars = (*(*(*classTable)[currentClassName].methods)[currentMethodName].variables);
		if (localVars.find(node->identifier_1->name) != localVars.end()) {
			// local var
			int localVarOffset = localVars[node->identifier_1->name].offset;
			std::cout << "  mov " << localVarOffset << "(%ebp), %eax" << std::endl;
			std::cout << "  mov 0(%esp), " << offset << "(%eax)" << std::endl;
			std::cout << "  add $4, %esp" << std::endl;
		}
		else {
			// member var
			auto memberOff = (*(*classTable)[className].members)[node->identifier_1->name].offset;
			std::cout << "  mov 8(%ebp), %eax" << std::endl;
			std::cout << "  mov " << memberOff << "(%eax), %eax" << std::endl;
			std::cout << "  mov 0(%esp), " << offset <<  "(%eax)" << std::endl;
			std::cout << "  add $4, %esp" << std::endl;	
		}
		
	}
}

void CodeGenerator::visitCallNode(CallNode* node) {
	node->visit_children(this);
	std::cout << "  add $4, %esp" << std::endl;
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
	int l1 = nextLabel();
	int l2 = nextLabel();

	node->expression->accept(this);

	std::cout << "  # IfElse" << std::endl;
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  mov $1, %eax" << std::endl;
	std::cout << "  cmp %edx, %eax" << std::endl;
	std::cout << "  je L" << l1 << std::endl;
	
	if (node->statement_list_2 != NULL){
		auto i = node->statement_list_2->begin();
		for(; i != node->statement_list_2->end(); ++i) {
			(*i)->accept(this);
		}
	}

	std::cout << "  jmp L" << l2 << std::endl;
	std::cout << "  L" << l1 << ":" << std::endl;

	auto j = node->statement_list_1->begin();
	for(; j != node->statement_list_1->end(); ++j) {
		(*j)->accept(this);
	}

	std::cout << "  L" << l2 << ":" << std::endl;
}

void CodeGenerator::visitWhileNode(WhileNode* node) {
	int l1 = nextLabel();
	int l2 = nextLabel();

	std::cout << "  # While" << std::endl;
	std::cout << "  L" << l1 << ":" << std::endl;

	node->expression->accept(this);

	std::cout << "  pop %edx" << std::endl;
	std::cout << "  cmp %edx, $0" << std::endl;
	std::cout << "  je L" << l2 << std::endl;

	auto i = node->statement_list->begin();
	for(; i != node->statement_list->end(); ++i) {
		(*i)->accept(this);
	}

	std::cout << " jmp L" << l1 << std::endl;
	std::cout << "  L" << l2 << ":" << std::endl;
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
	
	std::cout << "  push %eax" << std::endl;
	std::cout << "  push %ecx" << std::endl;
	std::cout << "  push %edx" << std::endl;
	
	node->visit_children(this);
	std::cout << "  push $printstr" << std::endl;
	std::cout << "  call printf" << std::endl;

	std::cout << "  add $4, %esp" << std::endl;

	std::cout << "  pop %edx" << std::endl;
	std::cout << "  pop %ecx" << std::endl;
	std::cout << "  pop %eax" << std::endl;
}

void CodeGenerator::visitDoWhileNode(DoWhileNode* node) {
	int l1 = nextLabel();

	std::cout << "  # DoWhile" << std::endl;
	std::cout << "  L" << l1 << ":" << std::endl;

	auto i = node->statement_list->begin();
	for(; i != node->statement_list->end(); ++i) {
		(*i)->accept(this);
	}

	node->expression->accept(this);

	std::cout << "  pop %edx" << std::endl;
	std::cout << "  cmp %edx, $1" << std::endl;
	std::cout << "  je L" << l1 << std::endl;
}

void CodeGenerator::visitPlusNode(PlusNode* node) {
	node->visit_children(this);
	std::cout << "  # Plus" << std::endl;
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  add %edx, %eax" << std::endl;
	std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMinusNode(MinusNode* node) {
	node->visit_children(this);
	std::cout << "  # Subtract" << std::endl;
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  sub %edx, %eax" << std::endl;
	std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitTimesNode(TimesNode* node) {
	node->visit_children(this);
	std::cout << "  # Times" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  pop %ecx" << std::endl;
	std::cout << "  imul %ecx" << std::endl;
	std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitDivideNode(DivideNode* node) {
	node->visit_children(this);
	std::cout << "  # Divide" << std::endl;
	std::cout << "  mov $0, %edx" << std::endl;
	std::cout << "  pop %ecx" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  idiv %ecx" << std::endl;
	std::cout << "  push %eax" << std::endl;
	// Dividend: EDX:EAX
	// Divisor:  ECX
	// Quotient: EAX
}

void CodeGenerator::visitGreaterNode(GreaterNode* node) {
	node->visit_children(this);
	int l1 = nextLabel();
	int l2 = nextLabel();

	std::cout << "  # Greater" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  cmp %eax, %edx" << std::endl;
	std::cout << "  jg L" << l1 << std::endl;
	std::cout << "  push $0" << std::endl;
	std::cout << "  jmp L" << l2 << std::endl;
	std::cout << "  L" << l1 << ": push $1" << std::endl;
	std::cout << "  L" << l2 << ": " << std::endl;
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
	node->visit_children(this);
	int l1 = nextLabel();
	int l2 = nextLabel();

	std::cout << "  # GreaterEqual" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  cmp %eax, %edx" << std::endl;
	std::cout << "  jge L" << l1 << std::endl;
	std::cout << "  push $0" << std::endl;
	std::cout << "  jmp L" << l2 << std::endl;
	std::cout << "  L" << l1 << ": push $1" << std::endl;
	std::cout << "  L" << l2 << ": " << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
	node->visit_children(this);
	int l1 = nextLabel();
	int l2 = nextLabel();

	std::cout << "  # Equal" << std::endl;
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  cmp %edx, %eax" << std::endl;
	std::cout << "  je L" << l1 << std::endl;
	std::cout << "  push $0" << std::endl;
	std::cout << "  jmp L" << l2 << std::endl;
	std::cout << "  L" << l1 << ": push $1" << std::endl;
	std::cout << "  L" << l2 << ": " << std::endl;
}

void CodeGenerator::visitAndNode(AndNode* node) {
	node->visit_children(this);
	std::cout << "  # And" << std::endl;	
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  and %edx, %eax" << std::endl;
	std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitOrNode(OrNode* node) {
	node->visit_children(this);
	std::cout << "  # Or" << std::endl;
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  or %edx, %eax" << std::endl;
	std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitNotNode(NotNode* node) {
	node->visit_children(this);
	std::cout << "  # Not" << std::endl;
	std::cout << "  mov %edx, $1" << std::endl;
	std::cout << "  pop %eax" << std::endl;
	std::cout << "  xor %edx, %eax" << std::endl;
	std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
	node->visit_children(this);
	std::cout << "  # Negation" << std::endl;
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  mov %eax, $0" << std::endl;
	std::cout << "  sub %edx, %eax" << std::endl;
	std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {

	std::cout << "  # MethodCall" << std::endl;
	std::cout << "  push %eax" << std::endl;
	std::cout << "  push %ecx" << std::endl;
	std::cout << "  push %edx" << std::endl;

	auto i = node->expression_list->rbegin();
	for(; i != node->expression_list->rend(); ++i) {
		(*i)->accept(this);
	}

	if (node->identifier_2 == NULL) {
		std::cout << "  push 8(%ebp)" <<std::endl;
		std::string className = currentClassName;
		
		while (className != "") {
			if ((*classTable)[className].methods->find(node->identifier_1->name) != (*classTable)[className].methods->end()) {
				std::cout << "  call " << className << "_" << node->identifier_1->name << std::endl;
				break;
			}
			className = (*classTable)[className].superClassName;
		}
	}
	else {
		
		auto localVars = (*(*(*classTable)[currentClassName].methods)[currentMethodName].variables);
		auto className = (*(*classTable)[currentClassName].members)[node->identifier_1->name].type.objectClassName;

		if (localVars.find(node->identifier_1->name) != localVars.end()) {
			// local var
			int localVarOffset = localVars[node->identifier_1->name].offset;
			std::cout << "  push " << localVarOffset << "(%ebp)" << std::endl;
		}
		else {
			// member var
			auto memberOff = (*(*classTable)[className].members)[node->identifier_1->name].offset;
			std::cout << "  mov 8(%ebp), %eax" << std::endl;
			std::cout << "  push " << memberOff << "(%eax)" << std::endl;
		}
		
		while (className != "") {
			if ((*classTable)[className].methods->find(node->identifier_2->name) != (*classTable)[className].methods->end()) {
				std::cout << "  call " << className << "_" << node->identifier_2->name << std::endl;
				break;
			}
			className = (*classTable)[className].superClassName;
		}
	}

	std::cout << "  mov %eax, %ecx" << std::endl;
	std::cout << "  add %esp, $" << 4 * (node->expression_list->size() + 1) << std::endl; 
	std::cout << "  pop %edx" << std::endl;
	std::cout << "  mov 4(%esp), %eax" << std::endl;
	std::cout << "  mov %ecx, 4(%esp)" << std::endl;
	std::cout << "  pop %ecx" << std::endl;

	// Below is deallocating parameters
	//std::cout << "  add %esp, " << 4 * (node->expression_list->size()+1) << std::endl; 
	//std::cout << "  pop %edx" << std::endl;
	//std::cout << "  pop %ecx" << std::endl;
	//std::cout << "  pop %eax" << std::endl;
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
	auto className = (*(*classTable)[currentClassName].members)[node->identifier_1->name].type.objectClassName;

	while (className != "") {
		if ((*classTable)[className].members->find(node->identifier_2->name) != (*classTable)[className].members->end()) {

			int offset = (*(*classTable)[className].members)[node->identifier_2->name].offset;
			// std::cout << "  mov 0(%esp), " << offset << "(%ebp)" << std::endl;
			// std::cout << "  add $4, %esp" << std::endl;
			
			auto className = currentClassName;
			auto localVars = (*(*(*classTable)[className].methods)[currentMethodName].variables);
			if (localVars.find(node->identifier_1->name) != localVars.end()) {
				// local var
				int localVarOffset = localVars[node->identifier_1->name].offset;
				std::cout << "  mov " << localVarOffset << "(%ebp), %eax" << std::endl;
				std::cout << "  push " << offset << "(%eax)" << std::endl;
			}
			else {
				// member var
				auto memberOff = (*(*classTable)[className].members)[node->identifier_1->name].offset;
				std::cout << "  mov 8(%ebp), %eax" << std::endl;
				std::cout << "  mov " << memberOff << "(%eax), %eax" << std::endl;
				std::cout << "  push " << offset <<  "(%eax)" << std::endl;
			}

			break;
		}
		className = (*classTable)[className].superClassName;
	}
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
	// check if local or member variable
	auto className = currentClassName;
	auto localVars = (*(*(*classTable)[className].methods)[currentMethodName].variables);
	if (localVars.find(node->identifier->name) != localVars.end()) {
		// local var
		int localVarOffset = localVars[node->identifier->name].offset;
		std::cout << "  push " << localVarOffset << "(%ebp)" << std::endl;
	}
	else {
		// member var
		auto memberOff = (*(*classTable)[className].members)[node->identifier->name].offset;
		std::cout << "  mov 8(%ebp), %eax" << std::endl;
		std::cout << "  push " << memberOff << "(%eax)" << std::endl;
	}
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
	std::cout << "  push $" << node->integer->value << std::endl;
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
	std::cout << "  push $" << node->integer->value << std::endl;
}

void CodeGenerator::visitNewNode(NewNode* node) {
	int size = (*classTable)[node->identifier->name].membersSize;

	// Constructor doesn't exist
	ClassInfo class_info = (*classTable)[node->identifier->name];

	if (class_info.methods->find(node->identifier->name) == class_info.methods->end()) {
		std::cout << "  push $" << size << std::endl;
		std::cout << "  call malloc" << std::endl;
		std::cout << "  add $4, %esp" << std::endl;
		std::cout << "  push %eax" << std::endl;
	}
	else {
		std::cout << "  # NewNode" << std::endl;
		std::cout << "  push %eax" << std::endl;
		std::cout << "  push %ecx" << std::endl;
		std::cout << "  push %edx" << std::endl;

		auto i = node->expression_list->rbegin();
		for(; i != node->expression_list->rend(); ++i) {
			(*i)->accept(this);
		}

		std::cout << "  push $" << size << std::endl;
		std::cout << "  call malloc" << std::endl;
		std::cout << "  add $4, %esp" << std::endl;
		std::cout << "  push %eax" << std::endl;

		std::cout << "  call " << node->identifier->name << "_" << node->identifier->name << std::endl;

		// Below is deallocating parameters
		std::cout << "  mov (%esp), %ecx" << std::endl;
		std::cout << "  add %esp, $" << 4 * (node->expression_list->size() + 1) << std::endl; 
		std::cout << "  pop %edx" << std::endl;
		std::cout << "  mov 4(%esp), %eax" << std::endl;
		std::cout << "  mov %ecx, 4(%esp)" << std::endl;
		std::cout << "  pop %ecx" << std::endl;
	}
}

// No need to do anything below
void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {
}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {
}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {
}

void CodeGenerator::visitNoneNode(NoneNode* node) {
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
}
