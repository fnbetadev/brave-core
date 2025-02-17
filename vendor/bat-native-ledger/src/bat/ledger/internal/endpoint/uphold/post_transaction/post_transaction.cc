/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ledger/internal/endpoint/uphold/post_transaction/post_transaction.h"

#include <utility>

#include "base/json/json_reader.h"
#include "base/json/json_writer.h"
#include "base/strings/stringprintf.h"
#include "bat/ledger/internal/endpoint/uphold/uphold_utils.h"
#include "bat/ledger/internal/ledger_impl.h"
#include "net/http/http_status_code.h"

using std::placeholders::_1;

namespace ledger {
namespace endpoint {
namespace uphold {

PostTransaction::PostTransaction(LedgerImpl* ledger):
    ledger_(ledger) {
  DCHECK(ledger_);
}

PostTransaction::~PostTransaction() = default;

std::string PostTransaction::GetUrl(const std::string& address) {
  const std::string path = base::StringPrintf(
      "/v0/me/cards/%s/transactions",
      address.c_str());

  return GetServerUrl(path);
}

std::string PostTransaction::GeneratePayload(
    const ::ledger::uphold::Transaction& transaction) {
  base::Value::Dict denomination;
  denomination.Set("amount", base::StringPrintf("%f", transaction.amount));
  denomination.Set("currency", "BAT");

  base::Value::Dict payload;
  payload.Set("destination", transaction.address);
  payload.Set("message", transaction.message);
  payload.Set("denomination", std::move(denomination));

  std::string json;
  base::JSONWriter::Write(payload, &json);
  return json;
}

mojom::Result PostTransaction::CheckStatusCode(const int status_code) {
  if (status_code == net::HTTP_UNAUTHORIZED) {
    BLOG(0, "Unauthorized access");
    return mojom::Result::EXPIRED_TOKEN;
  }

  if (status_code != net::HTTP_ACCEPTED) {
    BLOG(0, "Unexpected HTTP status: " << status_code);
    return mojom::Result::LEDGER_ERROR;
  }

  return mojom::Result::LEDGER_OK;
}

mojom::Result PostTransaction::ParseBody(const std::string& body,
                                         std::string* id) {
  DCHECK(id);

  absl::optional<base::Value> value = base::JSONReader::Read(body);
  if (!value || !value->is_dict()) {
    BLOG(0, "Invalid JSON");
    return mojom::Result::LEDGER_ERROR;
  }

  const base::Value::Dict& dict = value->GetDict();
  const auto* id_str = dict.FindString("id");
  if (!id_str) {
    BLOG(0, "Missing id");
    return mojom::Result::LEDGER_ERROR;
  }

  *id = *id_str;

  return mojom::Result::LEDGER_OK;
}

void PostTransaction::Request(
    const std::string& token,
    const std::string& address,
    const ::ledger::uphold::Transaction& transaction,
    PostTransactionCallback callback) {
  auto url_callback = std::bind(&PostTransaction::OnRequest,
      this,
      _1,
      callback);

  auto request = mojom::UrlRequest::New();
  request->url = GetUrl(address);
  request->content = GeneratePayload(transaction);
  request->headers = RequestAuthorization(token);
  request->content_type = "application/json; charset=utf-8";
  request->method = mojom::UrlMethod::POST;
  ledger_->LoadURL(std::move(request), url_callback);
}

void PostTransaction::OnRequest(const mojom::UrlResponse& response,
                                PostTransactionCallback callback) {
  ledger::LogUrlResponse(__func__, response);

  mojom::Result result = CheckStatusCode(response.status_code);

  if (result != mojom::Result::LEDGER_OK) {
    callback(result, "");
    return;
  }

  std::string id;
  result = ParseBody(response.body, &id);
  callback(result, id);
}

}  // namespace uphold
}  // namespace endpoint
}  // namespace ledger
