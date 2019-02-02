
#include "hookzz_internal.h"

#include "logging/logging.h"

#include "intercept_routing_handler.h"
#include "MultiThreadSupport/ThreadSupport.h"

#include "InterceptRoutingPlugin/intercept-routing-handler/intercept_routing_common_handler.h"

void instrument_call_forward_handler(RegisterContext *reg_ctx, HookEntry *entry) {
  // run the `dbi_call`, before the `instruction_address`
  if (entry->dbi_call) {
    DBICALL dbi_call;
    HookEntryInfo entry_info;
    entry_info.hook_id             = entry->id;
    entry_info.instruction_address = entry->instruction_address;
    dbi_call                       = entry->dbi_call;
    (*dbi_call)(reg_ctx, (const HookEntryInfo *)&entry_info);
  }

  // set prologue bridge next hop address with origin instructions that have been relocated(patched)
  set_routing_bridge_next_hop(reg_ctx, entry->relocated_origin_instructions);
}

void instrument_routing_dispatch(RegisterContext *reg_ctx, ClosureTrampolineEntry *closure_trampoline_entry) {
  DLOG("%s\n", "[*] catch prologue dispatch");
  HookEntry *entry = static_cast<HookEntry *>(closure_trampoline_entry->carry_data);
  instrument_call_forward_handler(reg_ctx, entry);
  return;
}