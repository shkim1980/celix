/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 *  KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef PUBSUB_PSA_TCP_MSG_HEADER_H_
#define PUBSUB_PSA_TCP_MSG_HEADER_H_

#define MARKER_START_PATTERN       (0x56781234)
#define MARKER_END_PATTERN         (0x67812345)

typedef struct pubsub_tcp_msg_header {
  uint32_t marker_start;
  uint32_t type; //msg type id (hash of fqn)
  uint32_t seqNr;
  uint8_t  major;
  uint8_t  minor;
  uint16_t padding;
  unsigned char originUUID[16];
  uint64_t sendtimeSeconds; //seconds since epoch
  uint64_t sendTimeNanoseconds; //ns since epoch
  uint32_t bufferSize; //Size of the buffer
  uint32_t marker_end;
} pubsub_tcp_msg_header_t;

#endif
