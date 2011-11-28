#include <iostream>
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

void printStatus(const IStatus* status)
{
  const ISC_STATUS* status_array = status->get();
  cout << "Status[]=" << status_array[0];
  for (int i = 1; i < ISC_STATUS_LENGTH; i++)
     cout << "," << status_array[i];
  cout << " (isSuccess=" << status->isSuccess() << ")\n";
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
  cout << "Before Init "; printStatus(status);
  status->init();
  cout << "After Init "; printStatus(status);

  return 0;
}
