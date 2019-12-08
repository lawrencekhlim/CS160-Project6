#include "codegeneration.hpp"

// CodeGenerator Visitor Functions: These are the functions
// you will complete to generate the x86 assembly code. Not
// all functions must have code, many may be left empty.




void CodeGenerator::visitProgramNode(ProgramNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitClassNode(ClassNode* node) {
    // WRITEME: Replace with code if necessary
    currentClassName = node->identifier_1->name;

}

void CodeGenerator::visitMethodNode(MethodNode* node) {
    currentMethodName = node->identifier->name;
    std::cout << currentClassName << "_" << currentMethodName << ": " << std::endl;
    std::cout << "  push %ebp" << std::endl;
    std::cout << "  mov %esp, %ebp" << std::endl;
    int offset = 4*(*(*classTable)[currentClassName].methods)[currentMethodName].localsSize;
    std::cout << "  sub %esp, " << offset << std::endl;

    std::cout << "  push %ebx" << std::endl;
    std::cout << "  push %esi" << std::endl;
    std::cout << "  push %edi" << std::endl;

    node->visit_children (this);

    std::cout << "  pop %edi" << std::endl;
    std::cout << "  pop %esi" << std::endl;
    std::cout << "  pop %ebx" << std::endl;
    
    std::cout << "  add %esp, " << offset << std::endl;
    std::cout << "  pop %ebp" << std::endl;
    std::cout << "  ret" << std::endl;
}

void CodeGenerator::visitMethodBodyNode(MethodBodyNode* node) {
    node->visit_children(this);
}

void CodeGenerator::visitParameterNode(ParameterNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitDeclarationNode(DeclarationNode* node) {
    // WRITEME: Replace with code if necessary
    // ClassNode
    // MethodNode - Handled inside MethodNode
}

void CodeGenerator::visitReturnStatementNode(ReturnStatementNode* node) {
    std::cout << "  # Return Statement" << std::endl;
    node->visit_children(this);
    std::cout << "  pop %eax" << endl;
}

void CodeGenerator::visitAssignmentNode(AssignmentNode* node) {
    // Several cases: 
    // Local variable
    // Class variable
    // Superclass variables
    node->visit_children (this);
    if (node->identifier_node_2 == NULL) {
        // Local variable
	// Find variable on stack first
	
        int offset = (*(*(*classTable)[currentClassName].methods)[currentMethodName].variables)[node->identifire_node_1->name].offset;
	std::cout << "  mov 0(%esp), " << offset << "(%ebp)" << std::endl;
	std::cout << "  add 4, %esp" << std::endl;
    } 
    else {
        // Class variable or superclass variable
	
    }    
}

void CodeGenerator::visitCallNode(CallNode* node) {
    node->visit_children(this);
}

void CodeGenerator::visitIfElseNode(IfElseNode* node) {
    int l1 = nextLabel();
    int l2 = nextLabel();

    node->expression->accept(this);

    std::cout << "  # IfElse" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  cmp %edx, 1" << std::endl;
    std::cout << "  je L" << l1 << std::endl;

    auto i = node->statement_list_2->begin();
    for(; i != node->statement_list_2->end(); ++i) {
        (*i)->accept(this);
    }

    std::cout << "  goto L" << l2 << std::endl;
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
    std::cout << "  cmp %edx, 0" << std::endl;
    std::cout << "  je L" << l2 << std::endl;

    auto i = node->statement_list->begin();
    for(; i != node->statement_list->end(); ++i) {
        (*i)->accept(this);
    }

    std::cout << "  goto L" << l1 << std::endl;
    std::cout << "  L" << l2 << ":" << std::endl;
}

void CodeGenerator::visitPrintNode(PrintNode* node) {
    // WRITEME: Replace with code if necessary
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
    std::cout << "  cmp %edx, 1" << std::endl;
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
    std::cout << "  mov %edx, 0" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  pop %ecx" << std::endl;
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
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cmp %eax, %edx" << l1 << std::endl;
    std::cout << "  jg L" << l1 << std::endl;
    std::cout << "  push 0" << std::endl;
    std::cout << "  goto L" << l2 << std::endl;
    std::cout << "  L" << l1 << ": push 1" << std::endl;
    std::cout << "  L" << l2 << ": " << std::endl;
}

void CodeGenerator::visitGreaterEqualNode(GreaterEqualNode* node) {
    node->visit_children(this);
    int l1 = nextLabel();
    int l2 = nextLabel();

    std::cout << "  # GreaterEqual" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cmp %eax, %edx" << l1 << std::endl;
    std::cout << "  jge L" << l1 << std::endl;
    std::cout << "  push 0" << std::endl;
    std::cout << "  goto L" << l2 << std::endl;
    std::cout << "  L" << l1 << ": push 1" << std::endl;
    std::cout << "  L" << l2 << ": " << std::endl;
}

void CodeGenerator::visitEqualNode(EqualNode* node) {
    node->visit_children(this);
    int l1 = nextLabel();
    int l2 = nextLabel();

    std::cout << "  # Equal" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  cmp %edx, %eax" << l1 << std::endl;
    std::cout << "  je L" << l1 << std::endl;
    std::cout << "  push 0" << std::endl;
    std::cout << "  goto L" << l2 << std::endl;
    std::cout << "  L" << l1 << ": push 1" << std::endl;
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
    std::cout << "  mov %edx, 1" << std::endl;
    std::cout << "  pop %eax" << std::endl;
    std::cout << "  xor %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitNegationNode(NegationNode* node) {
    node->visit_children(this);
    std::cout << "  # Negation" << std::endl;
    std::cout << "  pop %edx" << std::endl;
    std::cout << "  mov %eax, 0" << std::endl;
    std::cout << "  sub %edx, %eax" << std::endl;
    std::cout << "  push %eax" << std::endl;
}

void CodeGenerator::visitMethodCallNode(MethodCallNode* node) {
    IdentifierNode* identifier_1;
    IdentifierNode* identifier_2;
    std::list<ExpressionNode*>* expression_list;

    int l1 = nextLabel();
    int l2 = nextLabel();

    std::cout << "  # MethodCall" << std::endl;

    std::cout << "  push %eax" << std::endl;
    std::cout << "  push %ecx" << std::endl;
    std::cout << "  push %edx" << std::endl;

    auto i = node->expression_list->rbegin();
    for(; i != node->expression_list->rend(); ++i) {
        (*i)->accept(this);
    }

    std::cout << "  call " << identifier_2->name << std::endl;
    // TODO: Specify name space of identifier_2
    //
    // Below is deallocating parameters
    std::cout << "  add %esp, " << 4 * node->expression_list->size() << std::endl; 

    std::cout << "  pop %edx" << std::endl;
    std::cout << "  pop %ecx" << std::endl;
    std::cout << "  pop %eax" << std::endl;
}

void CodeGenerator::visitMemberAccessNode(MemberAccessNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitVariableNode(VariableNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerLiteralNode(IntegerLiteralNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitBooleanLiteralNode(BooleanLiteralNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNewNode(NewNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerTypeNode(IntegerTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitBooleanTypeNode(BooleanTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitObjectTypeNode(ObjectTypeNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitNoneNode(NoneNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIdentifierNode(IdentifierNode* node) {
    // WRITEME: Replace with code if necessary
}

void CodeGenerator::visitIntegerNode(IntegerNode* node) {
    // WRITEME: Replace with code if necessary
}
