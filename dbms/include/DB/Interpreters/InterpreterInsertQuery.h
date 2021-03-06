#pragma once

#include <DB/DataStreams/IBlockOutputStream.h>
#include <DB/DataStreams/BlockIO.h>
#include <DB/Interpreters/Context.h>
#include <DB/Interpreters/IInterpreter.h>


namespace DB
{


/** Интерпретирует запрос INSERT.
  */
class InterpreterInsertQuery : public IInterpreter
{
public:
	InterpreterInsertQuery(ASTPtr query_ptr_, Context & context_);

	/** Подготовить запрос к выполнению. Вернуть потоки блоков
	  * - поток, в который можно писать данные для выполнения запроса, если INSERT;
	  * - поток, из которого можно читать результат выполнения запроса, если SELECT и подобные;
	  * Или ничего, если запрос INSERT SELECT (самодостаточный запрос - не принимает входные данные, не отдаёт результат).
	  */
	BlockIO execute() override;

private:
	StoragePtr getTable();

	Block getSampleBlock();

	ASTPtr query_ptr;
	Context context;
};


}
