#include <iostream>
#include "stdio.h"
#include "string.h"
#include "ibase.h"
#include "firebird/Interface.h"
#include "firebird/Plugin.h"
#include "firebird/Provider.h"

/*
#include "firebird/ExternalEngine.h"
#include "firebird/Message.h"
#include "firebird/Timer.h"
#include "firebird/UdrCppEngine.h"
#include "firebird/UdrEngine.h"
*/

using namespace std;
using namespace Firebird;

int printStatus(const IStatus* status)
{
  const ISC_STATUS* status_array = status->get();
  cout << "  Status[]=" << status_array[0];
  for (int i = 1; i < ISC_STATUS_LENGTH; i++)
     cout << "," << status_array[i];
  cout << " (isSuccess=" << status->isSuccess() << ")\n";
  if (!status->isSuccess())
  {
    cout << "Error Occured:\n";
    isc_print_status(status_array);
  }
  return status->isSuccess();
}

int initDPB(int16_t page_size, char *dpb)
{
  int i=0;
  dpb[i++] = isc_dpb_version1;

  dpb[i++] = isc_dpb_sql_dialect;
  dpb[i++] = (char)1;
  dpb[i++] = (char)3;

  dpb[i++] = isc_dpb_page_size;
  dpb[i++] = (char)2;
  *(int16_t*)&dpb[i] = int16_t((isc_vax_integer)((char *)&page_size, 2));
  i += 2;

  dpb[i++] = isc_dpb_set_db_charset;
  dpb[i++] = (char)4;
  strncpy(&(dpb[i]), "UTF8", 4);
  i += 4;

  dpb[i++] = isc_dpb_user_name;
  dpb[i++] = (char)6;
  strncpy(&(dpb[i]), "SYSDBA", 6);
  i += 6;

  dpb[i++] = isc_dpb_password;
  dpb[i++] = (char)9;
  strncpy(&(dpb[i]), "masterkey", 9);
  i += 9;

  return i;
}

int main()
{
  cout << "FireTest - Playing with the new Firebird 3.0 OOAPI\n";

  static IMaster* master = fb_get_master_interface();
  cout << "Master version is " << master->getVersion() << "\n";

  IPluginModule* plugin_module = master->getModule();
  cout << "Plugin Module version is " << plugin_module->getVersion() << "\n";

  IProvider* provider = master->getDispatcher();
  cout << "Provider version is " << provider->getVersion() << "\n";

  IStatus* status = master->getStatus();
  cout << "Status version is " << status->getVersion() << "\n";
  cout << "Before Init\n"; printStatus(status);
  status->init();
  cout << "After Init\n"; printStatus(status);

  unsigned char dpb[48];
  int dpb_len = initDPB(8192, (char *)&dpb);

  cout << "Create Database\n";
  IAttachment* attachment = provider->createDatabase(status, "localhost:/tmp/test.fb3", dpb_len, (unsigned char *)&dpb);
  if (printStatus(status))
  {
      std::string SQL;

      cout << "Start Transaction\n";
      ITransaction* transaction = attachment->startTransaction(status, 0, NULL);
      printStatus(status);

      cout << "Create Table\n";
      SQL = "create table TEST(id INTEGER NOT NULL PRIMARY KEY, data VARCHAR(50))";
      transaction = attachment->execute(status, transaction, SQL.length(), SQL.c_str(), 3, 0, NULL, NULL); 
      printStatus(status);

      cout << "Commit Transaction\n";
      transaction->commit(status);
      printStatus(status);

      cout << "Allocate Statement\n";
      IStatement* statement = attachment->allocateStatement(status);
      printStatus(status);

      cout << "Start Transaction\n";
      transaction = attachment->startTransaction(status, 0, NULL);
      printStatus(status);

      cout << "Prepare Statement\n";
      SQL = "insert into TEST(id, data) values (1, 'Hello Firebird !')";
      statement->prepare(status, transaction, SQL.length(), SQL.c_str(), 3, IStatement::PREPARE_PREFETCH_TYPE);
      printStatus(status);

      cout << "Get Statement Type\n";
      const unsigned char items = (unsigned char)isc_info_sql_stmt_type;
      unsigned char buffer[2];
      statement->getInfo(status, 1, &items, 2, (unsigned char *)&buffer);
      printStatus(status);
      if (buffer[0] == isc_info_sql_stmt_insert)
        cout << " INSERT Statement Prepared !\n";

      cout << "Insert Something\n";
      transaction = statement->execute(status, transaction, isc_info_sql_stmt_insert, NULL, NULL);
      printStatus(status);

      cout << "Commit Transaction\n";
      transaction->commit(status);
      printStatus(status);

      cout << "Drop Database\n";
      attachment->drop(status);
      printStatus(status);
  }

  return 0;
}
