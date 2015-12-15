#define CATCH_CONFIG_MAIN //Make main function automatically
#include "catch.hpp"
#include "caster.hpp"
#include "SOMException.hpp"
#include<memory>
#include<string>
#include "zmq.hpp"
#include "zmq_utils.h"
#include <cstdio>
#include<functional>
#include "protobufSQLConverter.hpp"
#include "messageDatabaseDefinition.hpp"
#include "protobuf_sql_converter_test_message.pb.h"
#include "utilityFunctions.hpp"
#include "reactor.hpp"
#include<unistd.h>
#include "fileDataReceiver.hpp"
#include<Poco/Net/StreamSocket.h>
#include "tcpDataReceiver.hpp"
#include "transceiver.hpp"
//#include "ntripV1DataReceiver.hpp"

using namespace pylongps; //Use pylongps classes without alteration for now
using namespace pylongps_protobuf_sql_converter; //Use protobuf/sql converter test message

/*
TEST_CASE("ZMQ stream socket", "[talker]")
{
SECTION("ZMQ stream socket", "[talker]")
{
std::unique_ptr<zmq::context_t> context;

SOM_TRY
context.reset(new zmq::context_t());
SOM_CATCH("Error initializing context\n")



//Create socket to talk to TCP socket
std::unique_ptr<zmq::socket_t> testSocket;

SOM_TRY //Init socket
testSocket.reset(new zmq::socket_t(*context, ZMQ_STREAM));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
testSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
testSocket->connect("tcp://127.0.0.1:9001");
SOM_CATCH("Error connecting socket for registration with caster\n")

char connectionIdentityBuffer[256];
size_t connectionIdentitySize = 256;

SOM_TRY
testSocket->getsockopt(ZMQ_IDENTITY, connectionIdentityBuffer, &connectionIdentitySize);
SOM_CATCH("Error setting socket option\n")

std::string connectionIdentity(connectionIdentityBuffer, connectionIdentitySize);

printf("Socket ID size: %ld\n", connectionIdentity.size());

std::string testString = "this is a test string\n";
while(true)
{
SOM_TRY
testSocket->send(connectionIdentity.c_str(), connectionIdentity.size(), ZMQ_SNDMORE);
testSocket->send(testString.c_str(), testString.size());
SOM_CATCH("Error, unable to send message\n");
std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

}
}
*/

/*
TEST_CASE("File reading", "[Echo]")
{
SECTION("Echo section", "[echo]")
{
std::unique_ptr<zmq::context_t> context;

SOM_TRY
context.reset(new zmq::context_t());
SOM_CATCH("Error initializing context\n")

ntripV1DataReceiver receiver("207.4.96.200:2101", *context, "VRS_RTCM3",35.7928540, -78.6738100, );

fflush(stdout);

//Create socket to subscribe to caster
std::unique_ptr<zmq::socket_t> subscriberSocket;


SOM_TRY //Init socket
subscriberSocket.reset(new zmq::socket_t(*context, ZMQ_SUB));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
subscriberSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
subscriberSocket->connect(receiver.address().c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

SOM_TRY //Set filter to allow any published messages to be received
subscriberSocket->setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
SOM_CATCH("Error setting subscription for socket\n")


//Sleep for a few milliseconds to allow connection to stabilize so no messages are missed
std::this_thread::sleep_for(std::chrono::milliseconds(10));

while(true)
{
zmq::message_t messageBuffer;

SOM_TRY
subscriberSocket->recv(&messageBuffer);
SOM_CATCH("Error, unable to receive message\n")

fwrite(messageBuffer.data(), sizeof(char), messageBuffer.size(), stdout);
}

}
}

*/

TEST_CASE("Test argument parser", "[argument parser]")
{
SECTION("Make sure parser works as expected", "[argument parsing]")
{
const char *arguments[] = {"-", "test", "otherTest", "-C", "beta", "-o"};
int numberOfArguments = 6;

std::map<std::string, std::string> results = parseStringArguments((char **) arguments, numberOfArguments);

REQUIRE(results.size() == 3);
REQUIRE(results.at("") == "test");
REQUIRE(results.at("C") == "beta");
REQUIRE(results.at("o") == "");

}
}

TEST_CASE( "Test protobuf reflection", "[test]")
{

SECTION( "Print out fields of protobuf object")
{
//Make message to test on
protobuf_sql_converter_test_message testMessage;

client_query_request clientRequest0;
clientRequest0.set_max_number_of_results(101);

client_subquery subQuery0; //Shouldn't return any results
subQuery0.add_acceptable_classes(OFFICIAL);

sql_double_condition doubleCondition;
sql_integer_condition integerCondition;
sql_string_condition stringCondition;

client_subquery subQuery1; //Should return 1
subQuery1.add_acceptable_classes(COMMUNITY);
subQuery1.add_acceptable_formats(RTCM_V3_1);

doubleCondition.set_value(0.9); 
doubleCondition.set_relation(GREATER_THAN); 
auto doubleConditionBuffer = subQuery1.add_latitude_condition();
(*doubleConditionBuffer) = doubleCondition;
doubleCondition.set_value(1.1); 
doubleCondition.set_relation(LESS_THAN);
doubleConditionBuffer = subQuery1.add_latitude_condition(); 
(*doubleConditionBuffer) = doubleCondition;



doubleCondition.set_value(1.9); 
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_longitude_condition();
(*doubleConditionBuffer) = doubleCondition;
doubleCondition.set_value(2.1); 
doubleCondition.set_relation(LESS_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_longitude_condition();
(*doubleConditionBuffer) = doubleCondition;


doubleCondition.set_value(.01); //Up longer than .01 seconds
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_uptime_condition();
(*doubleConditionBuffer) = doubleCondition;

doubleCondition.set_value(2.9); 
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_expected_update_rate_condition();
(*doubleConditionBuffer) = doubleCondition;


stringCondition.set_value("testBasestat%");
stringCondition.set_relation(LIKE);
(*subQuery1.mutable_informal_name_condition()) = stringCondition;

integerCondition.set_value(20);
integerCondition.set_relation(EQUAL_TO);
auto integerConditionBuffer = subQuery1.add_base_station_id_condition();
(*integerConditionBuffer) = integerCondition;


base_station_radius_subquery circleSubQueryPart;
circleSubQueryPart.set_latitude(1.0);
circleSubQueryPart.set_longitude(2.0);
circleSubQueryPart.set_radius(10);
(*subQuery1.mutable_circular_search_region()) = circleSubQueryPart;

//Add subqueries to request
(*clientRequest0.add_subqueries()) = subQuery0;
(*clientRequest0.add_subqueries()) = subQuery1;

client_query_request clientRequest1 = clientRequest0;
client_query_request clientRequest2 = clientRequest0;
client_query_request clientRequest3 = clientRequest0;

//Add required submessage
(*testMessage.mutable_required_client_query_request()) = clientRequest0;

//Add 3 instances to repeated submessage field
(*testMessage.add_repeated_client_query_request()) = clientRequest1;
(*testMessage.add_repeated_client_query_request()) = clientRequest2;
(*testMessage.add_repeated_client_query_request()) = clientRequest3;

//Leave optional submessage empty

testMessage.set_required_int64(1);
testMessage.set_required_double(2.0);
testMessage.set_required_string("3");
testMessage.set_required_enum(TEST_REGISTERED_COMMUNITY);

testMessage.set_optional_int64(4);
testMessage.set_optional_double(5.0);
testMessage.set_optional_string("6");
testMessage.set_optional_enum(TEST_COMMUNITY);


testMessage.add_repeated_int64(7);
testMessage.add_repeated_int64(8);
testMessage.add_repeated_int64(9);
testMessage.add_repeated_double(10.0);
testMessage.add_repeated_double(11.0);
testMessage.add_repeated_double(12.0);
testMessage.add_repeated_string("13");
testMessage.add_repeated_string("14");
testMessage.add_repeated_string("15");
testMessage.add_repeated_enum(TEST_OFFICIAL);
testMessage.add_repeated_enum(TEST_REGISTERED_COMMUNITY);
testMessage.add_repeated_enum(TEST_COMMUNITY);

//Create sqlite interface for message
//Create database connection
sqlite3 *databaseConnection = nullptr;
REQUIRE(sqlite3_open_v2(":memory:", &databaseConnection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,NULL) == SQLITE_OK);
//REQUIRE(sqlite3_open_v2("./testDatabase.sqlite3", &databaseConnection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,NULL) == SQLITE_OK);

SOMScopeGuard databaseConnectionGuard([&]() {sqlite3_close_v2(databaseConnection);} );

std::unique_ptr<messageDatabaseDefinition> bob;

SOM_TRY
bob.reset(new messageDatabaseDefinition(*databaseConnection, *protobuf_sql_converter_test_message::descriptor()));
SOM_CATCH("Error, unable to initialize protobufSQLInterface\n")

SOM_TRY
bob->store(testMessage);
SOM_CATCH("Error storing message\n");

protobuf_sql_converter_test_message retrievedMessage;

SOM_TRY
bob->retrieve(testMessage.optional_int64(), retrievedMessage);
SOM_CATCH("Error retrieving message\n");

REQUIRE(testMessage.required_int64() == retrievedMessage.required_int64());
REQUIRE(testMessage.required_double() == retrievedMessage.required_double());
REQUIRE(testMessage.required_string() == retrievedMessage.required_string());
REQUIRE(testMessage.required_enum() == retrievedMessage.required_enum());

REQUIRE(retrievedMessage.has_optional_int64() == true);
REQUIRE(retrievedMessage.has_optional_double() == true);
REQUIRE(retrievedMessage.has_optional_string() == true);
REQUIRE(retrievedMessage.has_optional_enum() == true);

REQUIRE(testMessage.optional_int64() == retrievedMessage.optional_int64());
REQUIRE(testMessage.optional_double() == retrievedMessage.optional_double());
REQUIRE(testMessage.optional_string() == retrievedMessage.optional_string());
REQUIRE(testMessage.optional_enum() == retrievedMessage.optional_enum());

REQUIRE(retrievedMessage.repeated_int64_size() == testMessage.repeated_int64_size());
for(int i=0; i<retrievedMessage.repeated_int64_size(); i++)
{
REQUIRE(retrievedMessage.repeated_int64(i) == testMessage.repeated_int64(i));
}

REQUIRE(retrievedMessage.repeated_double_size() == testMessage.repeated_double_size());
for(int i=0; i<retrievedMessage.repeated_double_size(); i++)
{
REQUIRE(retrievedMessage.repeated_double(i) == testMessage.repeated_double(i));
}

REQUIRE(retrievedMessage.repeated_string_size() == testMessage.repeated_string_size());
for(int i=0; i<retrievedMessage.repeated_string_size(); i++)
{
REQUIRE(retrievedMessage.repeated_string(i) == testMessage.repeated_string(i));
}

REQUIRE(retrievedMessage.repeated_enum_size() == testMessage.repeated_enum_size());
for(int i=0; i<retrievedMessage.repeated_enum_size(); i++)
{
REQUIRE(retrievedMessage.repeated_enum(i) == testMessage.repeated_enum(i));
}

REQUIRE(testMessage.mutable_required_client_query_request()->has_max_number_of_results() == true);
REQUIRE(retrievedMessage.mutable_required_client_query_request()->has_max_number_of_results() == true);

REQUIRE(testMessage.mutable_required_client_query_request()->max_number_of_results() == retrievedMessage.mutable_required_client_query_request()->max_number_of_results());

REQUIRE(testMessage.mutable_required_client_query_request()->subqueries_size() == retrievedMessage.mutable_required_client_query_request()->subqueries_size());

REQUIRE(testMessage.repeated_client_query_request_size() == retrievedMessage.repeated_client_query_request_size());

REQUIRE(testMessage.mutable_required_client_query_request()->mutable_subqueries(1)->latitude_condition_size() == 2);

REQUIRE(retrievedMessage.mutable_required_client_query_request()->mutable_subqueries(1)->latitude_condition_size() == 2);

for(int i=0; i<testMessage.mutable_required_client_query_request()->mutable_subqueries(1)->latitude_condition_size(); i++)
{
REQUIRE(testMessage.mutable_required_client_query_request()->mutable_subqueries(1)->latitude_condition(i).relation()  == retrievedMessage.mutable_required_client_query_request()->mutable_subqueries(1)->latitude_condition(i).relation());
REQUIRE(testMessage.mutable_required_client_query_request()->mutable_subqueries(1)->latitude_condition(i).value()  == Approx(retrievedMessage.mutable_required_client_query_request()->mutable_subqueries(1)->latitude_condition(i).value()));
}

SOM_TRY
bob->update(testMessage.optional_int64(), 11, (int64_t) 1000);
testMessage.set_required_int64(1);
SOM_CATCH("Error, unable to update database\n");

SOM_TRY
bob->deleteMessage(testMessage.optional_int64());
SOM_CATCH("Error deleting message\n");

/*
//Print class name
const google::protobuf::Descriptor *messageDescriptor = testMessage.GetDescriptor();
const google::protobuf::Reflection* messageReflection = testMessage.GetReflection();
printf("Message name: %s\n", messageDescriptor->name().c_str());
printf("Field names/values: \n");
for(int i=0; i<messageDescriptor->field_count(); i++)
{
printf("%s: ", messageDescriptor->field(i)->name().c_str());
if(messageDescriptor->field(i)->is_required() || messageDescriptor->field(i)->is_required() )
{//Is singular
switch(messageDescriptor->field(i)->cpp_type())
{
case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
printf("%d\n", messageReflection->GetInt32(testMessage, messageDescriptor->field(i))); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
printf("%ld\n", messageReflection->GetInt64(testMessage, messageDescriptor->field(i))); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
printf("%u\n", messageReflection->GetUInt32(testMessage, messageDescriptor->field(i))); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
printf("%lu\n", messageReflection->GetUInt64(testMessage, messageDescriptor->field(i))); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
printf("%lf\n", messageReflection->GetDouble(testMessage, messageDescriptor->field(i))); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
printf("%f\n", messageReflection->GetFloat(testMessage, messageDescriptor->field(i))); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
printf("%d\n", messageReflection->GetBool(testMessage, messageDescriptor->field(i))); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
printf("%d\n", messageReflection->GetEnum(testMessage, messageDescriptor->field(i))->number()); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
printf("%s\n", messageReflection->GetString(testMessage, messageDescriptor->field(i)).c_str()); 
break;
case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
break;
}
}
else
{ //Is repeated or map
}

}
*/


}
}

/*

TEST_CASE("Test template deduction", "[template deduction]")
{
SECTION("See if type deduction works as I understand it", "[template deduction]")
{
int testInt = 5;
double testDouble = 10.0;
std::string testString = "ABC";
//printf("Testing type deduction\n");
//print(testInt);
//print(testDouble);
//print(testString);

}
}


TEST_CASE("Test Key Generation/format conversion functions", "[key management functions]")
{
SECTION("Test Key Generation/format conversion functions", "[key management functions]")
{

//Generate keys
std::string binaryPublicKey;
std::string binarySecretKey;

std::tie(binaryPublicKey, binarySecretKey) = generateSigningKeys();

REQUIRE(binaryPublicKey.size() == crypto_sign_PUBLICKEYBYTES);
REQUIRE(binarySecretKey.size() == crypto_sign_SECRETKEYBYTES);

std::string z85PublicKey;
std::string z85SecretKey;
SOM_TRY
z85PublicKey = convertStringToZ85Format(binaryPublicKey);
SOM_CATCH("Error, unable to convert to z85 format\n")

REQUIRE(z85PublicKey.size() == z85PublicKeySize);

SOM_TRY
z85SecretKey = convertStringToZ85Format(binarySecretKey);
SOM_CATCH("Error, unable to convert to z85 format\n")

REQUIRE(z85SecretKey.size() == z85SecretKeySize);

std::string decodedPublicKey;
std::string decodedSecretKey;

SOM_TRY
decodedPublicKey = convertStringFromZ85Format(z85PublicKey);
SOM_CATCH("Error, unable to convert string\n")

SOM_TRY
decodedSecretKey = convertStringFromZ85Format(z85SecretKey);
SOM_CATCH("Error, unable to convert string\n")

REQUIRE(decodedPublicKey == binaryPublicKey);
REQUIRE(decodedSecretKey == binarySecretKey);

}
}


TEST_CASE("Test ZMQ/Protobuf convenience functions", "[send/receive protobuf]")
{
SECTION("Test send/receive and RPC", "[send/receive protobuf]")
{
//Make ZMQ context
std::unique_ptr<zmq::context_t> context;

SOM_TRY
context.reset(new zmq::context_t);
SOM_CATCH("Error initializing ZMQ context\n")

std::unique_ptr<zmq::socket_t> testReplySocket; //TODO

SOM_TRY //Init socket
testReplySocket.reset(new zmq::socket_t(*context, ZMQ_REP));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
testReplySocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

std::string replySocketAddressString = "inproc://replySocketAddress";

SOM_TRY
testReplySocket->bind(replySocketAddressString.c_str());
SOM_CATCH("Error binding socket\n")

std::unique_ptr<zmq::socket_t> testRequestSocket; //TODO

SOM_TRY //Init socket
testRequestSocket.reset(new zmq::socket_t(*context, ZMQ_REQ));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
testRequestSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY
testRequestSocket->connect(replySocketAddressString.c_str());
SOM_CATCH("Error connecting socket\n")

SECTION("Test send/receive", "[send/receive protobuf]")
{
database_request testDatabaseRequest;
testDatabaseRequest.set_registration_connection_id("Hello!");
testDatabaseRequest.add_delete_base_station_ids(-10);
testDatabaseRequest.set_base_station_to_update_id(20);
testDatabaseRequest.set_real_update_rate(31.1);

SOM_TRY
sendProtobufMessage(*testRequestSocket, testDatabaseRequest);
SOM_CATCH("Error, unable to send protobuf message\n")

//Attempt to receive the send object
database_request receivedMessage;
bool messageReceived = false;
bool messageDeserialized = false;

SOM_TRY
std::tie(messageReceived, messageDeserialized) = receiveProtobufMessage(*testReplySocket, receivedMessage);
SOM_CATCH("Error receiving message object\n") //TODO

REQUIRE(messageReceived == true);
REQUIRE(messageDeserialized == true);
REQUIRE(receivedMessage.has_registration_connection_id() == true);
REQUIRE(receivedMessage.delete_base_station_ids_size() == 1);
REQUIRE(receivedMessage.has_base_station_to_update_id() == true);
REQUIRE(receivedMessage.has_real_update_rate() == true);
REQUIRE(receivedMessage.registration_connection_id() == "Hello!");
REQUIRE(receivedMessage.delete_base_station_ids(0) == -10);
REQUIRE(receivedMessage.base_station_to_update_id() == 20);
REQUIRE(receivedMessage.real_update_rate() == Approx(31.1));
}


}

}


TEST_CASE( "Caster Initializes", "[test]")
{

SECTION( "Create and destroy caster")
{
//Make ZMQ context
std::unique_ptr<zmq::context_t> context;

SOM_TRY
context.reset(new zmq::context_t);
SOM_CATCH("Error initializing ZMQ context\n")

//Generate keys to use
std::string casterPublicKey;
std::string casterPrivateKey;
std::tie(casterPublicKey, casterPrivateKey) = generateSigningKeys();

//Generate key manager signing key
unsigned char keyManagerPublicKeyArray[crypto_sign_PUBLICKEYBYTES];
unsigned char keyManagerSecretKeyArray[crypto_sign_SECRETKEYBYTES];
crypto_sign_keypair(keyManagerPublicKeyArray, keyManagerSecretKeyArray);

std::string keyManagerPublicKey((const char *) keyManagerPublicKeyArray, crypto_sign_PUBLICKEYBYTES);
std::string keyManagerSecretKey((const char *) keyManagerSecretKeyArray, crypto_sign_SECRETKEYBYTES);

caster myCaster(context.get(), 0,9001,9002,9003, 9004, 9005, 9006, casterPublicKey, casterPrivateKey, keyManagerPublicKey, std::vector<std::string>(0), std::vector<std::string>(0), std::vector<std::string>(0));

REQUIRE( true == true);
}
}

class testClass
{
public:
testClass(int inputNumber)
{
number = inputNumber;
}

int getNumber()
{
return number;
}

void setNumber(int inputNumber)
{
number = inputNumber; 
}

int number;
};

template <class classType> class functionCaller
{
public:
void setFunctionToCall(const std::function<int(classType*)> inputFunctionPointer)
{
memberFunctionPointer = inputFunctionPointer;
}

void callFunction(classType &inputTestClass)
{
//printf("Called with value: %d\n", memberFunctionPointer(&inputTestClass));
}

std::function<int(classType*)> memberFunctionPointer;
};


TEST_CASE( "Test member function pointers", "[test]")
{

SECTION( "Call member function pointer")
{
//Create two objects
testClass test0(0);
testClass test1(9);

functionCaller<testClass> functionCaller;
functionCaller.setFunctionToCall(&testClass::getNumber);

//std::function<void(testClass*)> f_add_display = &testClass::getNumber;

functionCaller.callFunction(test0);
functionCaller.callFunction(test1);

}
}

TEST_CASE( "Test protobufSQLConverter", "[test]")
{

SECTION( "Set/get fields")
{
//Create database connection
sqlite3 *databaseConnection = nullptr;
REQUIRE(sqlite3_open_v2(":memory:", &databaseConnection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,NULL) == SQLITE_OK);
//REQUIRE(sqlite3_open_v2("./testDatabase", &databaseConnection, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE,NULL) == SQLITE_OK);

SOMScopeGuard databaseConnectionGuard([&]() {sqlite3_close_v2(databaseConnection);} );

//Make sure foreign key constractions are active for this connection
REQUIRE(sqlite3_exec(databaseConnection, "PRAGMA foreign_keys = on;", NULL, NULL, NULL) == SQLITE_OK);

//Initialize protobufSQLConverter
protobufSQLConverter<protobuf_sql_converter_test_message> testConverter(databaseConnection, "protobuf_sql_converter_test_message_table");

//Add REQUIRED fields
//int64 primary key
testConverter.addField(PYLON_GPS_GEN_REQUIRED_INT64_FIELD(protobuf_sql_converter_test_message, required_int64, "required_int64"), true); 
testConverter.addField(PYLON_GPS_GEN_REQUIRED_DOUBLE_FIELD(protobuf_sql_converter_test_message, required_double, "required_double" )); 
testConverter.addField(PYLON_GPS_GEN_REQUIRED_STRING_FIELD(protobuf_sql_converter_test_message, required_string, "required_string"));
testConverter.addField(PYLON_GPS_GEN_REQUIRED_ENUM_FIELD(protobuf_sql_converter_test_message, test_enum, required_enum, "required_enum"));



//Add OPTIONAL fields
testConverter.addField(PYLON_GPS_GEN_OPTIONAL_INT64_FIELD(protobuf_sql_converter_test_message, optional_int64, "optional_int64"));
testConverter.addField(PYLON_GPS_GEN_OPTIONAL_DOUBLE_FIELD(protobuf_sql_converter_test_message, optional_double, "optional_double" )); 
testConverter.addField(PYLON_GPS_GEN_OPTIONAL_STRING_FIELD(protobuf_sql_converter_test_message, optional_string, "optional_string"));
testConverter.addField(PYLON_GPS_GEN_OPTIONAL_ENUM_FIELD(protobuf_sql_converter_test_message, test_enum, optional_enum, "optional_enum"));


//These are left null when the message is made (not set)
testConverter.addField(PYLON_GPS_GEN_OPTIONAL_INT64_FIELD(protobuf_sql_converter_test_message, null_optional_int64, "null_optional_int64"));
testConverter.addField(PYLON_GPS_GEN_OPTIONAL_DOUBLE_FIELD(protobuf_sql_converter_test_message, null_optional_double, "null_optional_double" )); 
testConverter.addField(PYLON_GPS_GEN_OPTIONAL_STRING_FIELD(protobuf_sql_converter_test_message, null_optional_string, "null_optional_string"));
testConverter.addField(PYLON_GPS_GEN_OPTIONAL_ENUM_FIELD(protobuf_sql_converter_test_message, test_enum, null_optional_enum, "null_optional_enum"));


//Add REPEATED fields
testConverter.addField(PYLON_GPS_GEN_REPEATED_INT64_FIELD(protobuf_sql_converter_test_message, repeated_int64,  "repeated_int64_table", "repeated_int64", "key"));
testConverter.addField(PYLON_GPS_GEN_REPEATED_DOUBLE_FIELD(protobuf_sql_converter_test_message, repeated_double,  "repeated_double_table", "repeated_double", "key"));
testConverter.addField(PYLON_GPS_GEN_REPEATED_STRING_FIELD(protobuf_sql_converter_test_message, repeated_string,  "repeated_string_table", "repeated_string", "key"));
testConverter.addField(PYLON_GPS_GEN_REPEATED_ENUM_FIELD(protobuf_sql_converter_test_message, test_enum, repeated_enum,  "repeated_enum_table", "repeated_enum", "key"));



//Automatically generate tables
testConverter.createTables();

//Make message to test on
protobuf_sql_converter_test_message testMessage;
testMessage.set_required_int64(1);
testMessage.set_required_double(2.0);
testMessage.set_required_string("3");
testMessage.set_required_enum(TEST_REGISTERED_COMMUNITY);

testMessage.set_optional_int64(4);
testMessage.set_optional_double(5.0);
testMessage.set_optional_string("6");
testMessage.set_optional_enum(TEST_COMMUNITY);


testMessage.add_repeated_int64(7);
testMessage.add_repeated_int64(8);
testMessage.add_repeated_int64(9);
testMessage.add_repeated_double(10.0);
testMessage.add_repeated_double(11.0);
testMessage.add_repeated_double(12.0);
testMessage.add_repeated_string("13");
testMessage.add_repeated_string("14");
testMessage.add_repeated_string("15");
testMessage.add_repeated_enum(TEST_OFFICIAL);
testMessage.add_repeated_enum(TEST_REGISTERED_COMMUNITY);
testMessage.add_repeated_enum(TEST_COMMUNITY);

printf("Manual attempt %ld\n", testMessage.required_int64());
std::function<const ::google::protobuf::int64 &(const protobuf_sql_converter_test_message* )> functionPointer = (&protobuf_sql_converter_test_message::required_int64) ;
printf("Fellow\n");
printf("Try %ld\n", functionPointer(&testMessage));
printf("Yello\n");

//Print out all of the fields we added
testConverter.print(testMessage);


//Attempt to write to the database
testConverter.store(testMessage);

printf("Made it this far\n");

std::vector<protobuf_sql_converter_test_message> retrievedValues;
std::vector<::google::protobuf::int64> keys = {1};

SOM_TRY
retrievedValues = testConverter.retrieve(keys);
SOM_CATCH("Error retrieving values")

REQUIRE(retrievedValues.size() == 1);

REQUIRE(retrievedValues[0].required_int64() == 1);
REQUIRE(retrievedValues[0].required_double() == Approx(2.0));
REQUIRE(retrievedValues[0].required_string() == "3");
REQUIRE(retrievedValues[0].required_enum() == TEST_REGISTERED_COMMUNITY);

REQUIRE(retrievedValues[0].has_optional_int64() == true);
REQUIRE(retrievedValues[0].has_optional_double() == true);
REQUIRE(retrievedValues[0].has_optional_string() == true);
REQUIRE(retrievedValues[0].has_optional_enum() == true);

REQUIRE(retrievedValues[0].has_null_optional_int64() == false);
REQUIRE(retrievedValues[0].has_null_optional_double() == false);
REQUIRE(retrievedValues[0].has_null_optional_string() == false);
REQUIRE(retrievedValues[0].has_null_optional_enum() == false);

REQUIRE(retrievedValues[0].optional_int64() == 4);
REQUIRE(retrievedValues[0].optional_double() == Approx(5.0));
REQUIRE(retrievedValues[0].optional_string() == "6");
REQUIRE(retrievedValues[0].optional_enum() == TEST_COMMUNITY);

REQUIRE(retrievedValues[0].repeated_int64_size() == 3);
REQUIRE(retrievedValues[0].repeated_int64(0) == 7);
REQUIRE(retrievedValues[0].repeated_int64(1) == 8);
REQUIRE(retrievedValues[0].repeated_int64(2) == 9);
REQUIRE(retrievedValues[0].repeated_double_size() == 3);
REQUIRE(retrievedValues[0].repeated_double(0) == Approx(10.0));
REQUIRE(retrievedValues[0].repeated_double(1) == Approx(11.0));
REQUIRE(retrievedValues[0].repeated_double(2) == Approx(12.0));
REQUIRE(retrievedValues[0].repeated_string_size() == 3);
REQUIRE(retrievedValues[0].repeated_string(0) == "13");
REQUIRE(retrievedValues[0].repeated_string(1) == "14");
REQUIRE(retrievedValues[0].repeated_string(2) == "15");
REQUIRE(retrievedValues[0].repeated_enum_size() == 3);
REQUIRE(retrievedValues[0].repeated_enum(0) == TEST_OFFICIAL);
REQUIRE(retrievedValues[0].repeated_enum(1) == TEST_REGISTERED_COMMUNITY);
REQUIRE(retrievedValues[0].repeated_enum(2) == TEST_COMMUNITY);

//Test updating a field in the database
testConverter.update(fieldValue((::google::protobuf::int64) 1), "required_double", 16.0);

//Retrieve and check if update went through
SOM_TRY
retrievedValues = testConverter.retrieve(keys);
SOM_CATCH("Error retrieving values")

REQUIRE(retrievedValues.size() == 1);

REQUIRE(retrievedValues[0].required_int64() == 1);
REQUIRE(retrievedValues[0].required_double() == Approx(16.0));
REQUIRE(retrievedValues[0].required_string() == "3");
REQUIRE(retrievedValues[0].required_enum() == TEST_REGISTERED_COMMUNITY);

REQUIRE(retrievedValues[0].has_optional_int64() == true);
REQUIRE(retrievedValues[0].has_optional_double() == true);
REQUIRE(retrievedValues[0].has_optional_string() == true);
REQUIRE(retrievedValues[0].has_optional_enum() == true);

REQUIRE(retrievedValues[0].has_null_optional_int64() == false);
REQUIRE(retrievedValues[0].has_null_optional_double() == false);
REQUIRE(retrievedValues[0].has_null_optional_string() == false);
REQUIRE(retrievedValues[0].has_null_optional_enum() == false);

REQUIRE(retrievedValues[0].optional_int64() == 4);
REQUIRE(retrievedValues[0].optional_double() == Approx(5.0));
REQUIRE(retrievedValues[0].optional_string() == "6");
REQUIRE(retrievedValues[0].optional_enum() == TEST_COMMUNITY);

REQUIRE(retrievedValues[0].repeated_int64_size() == 3);
REQUIRE(retrievedValues[0].repeated_int64(0) == 7);
REQUIRE(retrievedValues[0].repeated_int64(1) == 8);
REQUIRE(retrievedValues[0].repeated_int64(2) == 9);
REQUIRE(retrievedValues[0].repeated_double_size() == 3);
REQUIRE(retrievedValues[0].repeated_double(0) == Approx(10.0));
REQUIRE(retrievedValues[0].repeated_double(1) == Approx(11.0));
REQUIRE(retrievedValues[0].repeated_double(2) == Approx(12.0));
REQUIRE(retrievedValues[0].repeated_string_size() == 3);
REQUIRE(retrievedValues[0].repeated_string(0) == "13");
REQUIRE(retrievedValues[0].repeated_string(1) == "14");
REQUIRE(retrievedValues[0].repeated_string(2) == "15");
REQUIRE(retrievedValues[0].repeated_enum_size() == 3);
REQUIRE(retrievedValues[0].repeated_enum(0) == TEST_OFFICIAL);
REQUIRE(retrievedValues[0].repeated_enum(1) == TEST_REGISTERED_COMMUNITY);
REQUIRE(retrievedValues[0].repeated_enum(2) == TEST_COMMUNITY);


//Delete the object from the database
SOM_TRY
testConverter.deleteObjects(keys);
SOM_CATCH("Error deleting objects")

//Test if delete took
SOM_TRY
retrievedValues = testConverter.retrieve(keys);
SOM_CATCH("Error retrieving values")

REQUIRE(retrievedValues.size() == 0);
}
}

TEST_CASE( "Test unauthenticated stream registration and caster transceiver", "[test]")
{

SECTION( "Build a caster")
{
//Make ZMQ context
std::unique_ptr<zmq::context_t> context;

SOM_TRY
context.reset(new zmq::context_t);
SOM_CATCH("Error initializing ZMQ context\n")

//Generate keys to use
std::string casterPublicKey;
std::string casterPrivateKey;
std::tie(casterPublicKey, casterPrivateKey) = generateSigningKeys();

//Generate key manager signing key
unsigned char keyManagerPublicKeyArray[crypto_sign_PUBLICKEYBYTES];
unsigned char keyManagerSecretKeyArray[crypto_sign_SECRETKEYBYTES];
crypto_sign_keypair(keyManagerPublicKeyArray, keyManagerSecretKeyArray);

std::string keyManagerPublicKey((const char *) keyManagerPublicKeyArray, crypto_sign_PUBLICKEYBYTES);
std::string keyManagerSecretKey((const char *) keyManagerSecretKeyArray, crypto_sign_SECRETKEYBYTES);

Poco::Int64 casterID = 989;
int registrationPort = 9010;
int clientRequestPort = 9013;
int clientPublishingPort = 9014;
int proxyPublishingPort = 9015;
int streamStatusNotificationPort = 9016;
int keyManagementPort = 9017;


caster myCaster(context.get(), casterID,registrationPort,clientRequestPort, clientPublishingPort, proxyPublishingPort, streamStatusNotificationPort, keyManagementPort, casterPublicKey, casterPrivateKey, keyManagerPublicKey, std::vector<std::string>(0), std::vector<std::string>(0), std::vector<std::string>(0));
//caster myCaster(context.get(), casterID,registrationPort,clientRequestPort, clientPublishingPort, proxyPublishingPort, streamStatusNotificationPort, decodedCasterPublicKey, decodedCasterSecretKey, keyManagerPublicKey, std::vector<std::string>(0), std::vector<std::string>(0), std::vector<std::string>(0), "testSQLDatabase.db");

SECTION( "Register a unauthenticated stream")
{
std::unique_ptr<zmq::socket_t> testMessagePublisher;

SOM_TRY //Init socket
testMessagePublisher.reset(new zmq::socket_t(*context, ZMQ_PUB));
SOM_CATCH("Error making socket\n")

std::string ZMQPubSocketAddressString = "tcp://*:10001";

SOM_TRY
testMessagePublisher->bind(ZMQPubSocketAddressString.c_str());
SOM_CATCH("Error binding socket\n")

std::unique_ptr<transceiver> com;

SOM_TRY
com.reset(new transceiver(*context));
SOM_CATCH("Error initializing transceiver\n")

//ZMQ Pub receiver
std::string pubDataReceiverAddress;

SOM_TRY
pubDataReceiverAddress = com->createZMQPubDataReceiver("127.0.0.1:10001");
SOM_CATCH("Error, unable to create data receiver\n")

//Caster data sender
std::string senderURI;
SOM_TRY 
senderURI = com->createPylonGPSV2DataSender(pubDataReceiverAddress, "127.0.0.1:" +std::to_string(registrationPort), 1.0, 2.0, RTCM_V3_1, "testBasestation", 3.0);
SOM_CATCH("Error making caster sender\n")

std::this_thread::sleep_for(std::chrono::milliseconds(10));


SECTION( "Send a query, Send/receive update")
{
//Send a query and see if we get a valid response
client_query_request queryRequest; //Empty request should return all
client_query_reply queryReply;

SOM_TRY
queryReply = transceiver::queryPylonGPSV2Caster(queryRequest, "127.0.0.1:" + std::to_string(clientRequestPort), 5000, *context);
SOM_CATCH("Error querying caster\n")


REQUIRE(queryReply.IsInitialized() == true);
REQUIRE(queryReply.has_caster_id() == true);
REQUIRE(queryReply.caster_id() == casterID);
REQUIRE(queryReply.has_failure_reason() == false);
REQUIRE(queryReply.base_stations_size() == 1);

auto replyBaseStationInfo = queryReply.base_stations(0);
REQUIRE(replyBaseStationInfo.has_latitude());
REQUIRE(replyBaseStationInfo.latitude() == Approx(1.0));
REQUIRE(replyBaseStationInfo.has_longitude());
REQUIRE(replyBaseStationInfo.longitude() == Approx(2.0));
REQUIRE(replyBaseStationInfo.has_expected_update_rate());
REQUIRE(replyBaseStationInfo.expected_update_rate() == Approx(3.0));
REQUIRE(replyBaseStationInfo.has_message_format());
REQUIRE(replyBaseStationInfo.message_format() == RTCM_V3_1);
REQUIRE(replyBaseStationInfo.has_informal_name());
REQUIRE(replyBaseStationInfo.informal_name() == "testBasestation");
REQUIRE(replyBaseStationInfo.has_base_station_id());

//Store station ID so it can be used for later checks
auto baseStationID = replyBaseStationInfo.base_station_id();

//Do it again with a more complex query

//Let the base station entry age a bit, so we can check uptime
std::this_thread::sleep_for(std::chrono::milliseconds(100));

client_query_request clientRequest0; //Empty request should return all
client_query_reply clientReply0;

client_subquery subQuery0; //Shouldn't return any results
subQuery0.add_acceptable_classes(OFFICIAL);

sql_double_condition doubleCondition;
sql_integer_condition integerCondition;
sql_string_condition stringCondition;

client_subquery subQuery1; //Should return 1
subQuery1.add_acceptable_classes(COMMUNITY);
subQuery1.add_acceptable_formats(RTCM_V3_1);

doubleCondition.set_value(0.9); 
doubleCondition.set_relation(GREATER_THAN); 
auto doubleConditionBuffer = subQuery1.add_latitude_condition();
(*doubleConditionBuffer) = doubleCondition;
doubleCondition.set_value(1.1); 
doubleCondition.set_relation(LESS_THAN);
doubleConditionBuffer = subQuery1.add_latitude_condition(); 
(*doubleConditionBuffer) = doubleCondition;



doubleCondition.set_value(1.9); 
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_longitude_condition();
(*doubleConditionBuffer) = doubleCondition;
doubleCondition.set_value(2.1); 
doubleCondition.set_relation(LESS_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_longitude_condition();
(*doubleConditionBuffer) = doubleCondition;


doubleCondition.set_value(.01); //Up longer than .01 seconds
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_uptime_condition();
(*doubleConditionBuffer) = doubleCondition;

doubleCondition.set_value(2.9); 
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_expected_update_rate_condition();
(*doubleConditionBuffer) = doubleCondition;


stringCondition.set_value("testBasestat%");
stringCondition.set_relation(LIKE);
(*subQuery1.mutable_informal_name_condition()) = stringCondition;

integerCondition.set_value(baseStationID);
integerCondition.set_relation(EQUAL_TO);
auto integerConditionBuffer = subQuery1.add_base_station_id_condition();
(*integerConditionBuffer) = integerCondition;


base_station_radius_subquery circleSubQueryPart;
circleSubQueryPart.set_latitude(1.0);
circleSubQueryPart.set_longitude(2.0);
circleSubQueryPart.set_radius(10);
(*subQuery1.mutable_circular_search_region()) = circleSubQueryPart;

//Add subqueries to request
(*clientRequest0.add_subqueries()) = subQuery0;
(*clientRequest0.add_subqueries()) = subQuery1;

SOM_TRY
clientReply0 = transceiver::queryPylonGPSV2Caster(clientRequest0, "127.0.0.1:" + std::to_string(clientRequestPort), 5000, *context);
SOM_CATCH("Error querying caster\n")


REQUIRE(queryReply.IsInitialized() == true);
REQUIRE(queryReply.has_caster_id() == true);
REQUIRE(queryReply.caster_id() == casterID);
REQUIRE(queryReply.has_failure_reason() == false);
REQUIRE(queryReply.base_stations_size() == 1);

replyBaseStationInfo = queryReply.base_stations(0);
REQUIRE(replyBaseStationInfo.has_latitude());
REQUIRE(replyBaseStationInfo.latitude() == Approx(1.0));
REQUIRE(replyBaseStationInfo.has_longitude());
REQUIRE(replyBaseStationInfo.longitude() == Approx(2.0));
REQUIRE(replyBaseStationInfo.has_expected_update_rate());
REQUIRE(replyBaseStationInfo.expected_update_rate() == Approx(3.0));
REQUIRE(replyBaseStationInfo.has_message_format());
REQUIRE(replyBaseStationInfo.message_format() == RTCM_V3_1);
REQUIRE(replyBaseStationInfo.has_informal_name());
REQUIRE(replyBaseStationInfo.informal_name() == "testBasestation");

//Base station registered, so subscribe and see if we get the next message send

//Make transciever subscribe to our basestation in the caster and forward the data to a ZMQ PUB socket
std::string receiverConnectionString;

SOM_TRY
receiverConnectionString = com->createPylonGPSV2DataReceiver("127.0.0.1:"+std::to_string(clientPublishingPort), casterID, baseStationID); 
SOM_CATCH("Error, unable to make receiver\n")

std::string zmqSenderID;
int zmqSenderPortNumber = keyManagementPort+10;

SOM_TRY
zmqSenderID = com->createZMQDataSender(receiverConnectionString, zmqSenderPortNumber);
SOM_CATCH("Error, unable to create zmq data sender\n")

//Create socket to subscribe to zmq data sender
std::unique_ptr<zmq::socket_t> subscriberSocket;

SOM_TRY //Init socket
subscriberSocket.reset(new zmq::socket_t(*context, ZMQ_SUB));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
subscriberSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to data sender
std::string connectionString = "tcp://127.0.0.1:" +std::to_string(zmqSenderPortNumber);
subscriberSocket->connect(connectionString.c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

SOM_TRY //Set filter to allow any published messages to be received
subscriberSocket->setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
SOM_CATCH("Error setting subscription for socket\n")

//Sleep for a few milliseconds to allow connection to stabilize so no messages are missed
std::this_thread::sleep_for(std::chrono::milliseconds(10));

//Send message to caster and see if we get it back with the subscriber
std::string testString = "This is a test string\n";
SOM_TRY
testMessagePublisher->send(testString.c_str(), testString.size());
SOM_CATCH("Error sending message to caster\n")

std::unique_ptr<zmq::message_t> messageBuffer;

SOM_TRY
messageBuffer.reset(new zmq::message_t);
SOM_CATCH("Error initializing ZMQ message")

REQUIRE(subscriberSocket->recv(messageBuffer.get()) == true);

//Check message format
REQUIRE(messageBuffer->size() == testString.size());

REQUIRE(std::string((const char *) messageBuffer->data(), messageBuffer->size()) == testString);

} //Send/receive update

} //Register unauthenticated stream


SECTION( "Register a official authenticated stream")
{
/////Create and register a official signing key////////

//Generate the official signing key
unsigned char officialSigningPublicKeyArray[crypto_sign_PUBLICKEYBYTES];
unsigned char officialSigningSecretKeyArray[crypto_sign_SECRETKEYBYTES];
crypto_sign_keypair(officialSigningPublicKeyArray, officialSigningSecretKeyArray);

std::string officialSigningPublicKey((const char *) keyManagerPublicKeyArray, crypto_sign_PUBLICKEYBYTES);
std::string officialSigningSecretKey((const char *) keyManagerSecretKeyArray, crypto_sign_SECRETKEYBYTES);
 
//Construct request to add the key
Poco::Timestamp currentTime;
auto timeValue = currentTime.epochMicroseconds() + 1000000*60; //Times out a minute from now

std::string serializedChangesToMake;
key_status_changes changesToMake;
changesToMake.add_official_signing_keys_to_add(officialSigningPublicKey);
changesToMake.add_official_signing_keys_to_add_valid_until(timeValue);

changesToMake.SerializeToString(&serializedChangesToMake);

//Sign changes to make with keyManagerSecretKey
unsigned char changesSignatureArray[crypto_sign_BYTES];
std::string changesSignatureString;

crypto_sign_detached(changesSignatureArray, nullptr, (const unsigned char *) serializedChangesToMake.c_str(), serializedChangesToMake.size(), (const unsigned char *) keyManagerSecretKey.c_str());
changesSignatureString = std::string((const char *) changesSignatureArray, crypto_sign_BYTES);

signature keyManagerSignature;
keyManagerSignature.set_public_key(keyManagerPublicKey);
keyManagerSignature.set_cryptographic_signature(changesSignatureString);

std::string serializedKeyManagementRequest;
key_management_request keyManagementRequest;
keyManagementRequest.set_serialized_key_status_changes(serializedChangesToMake);
*keyManagementRequest.mutable_signature() = keyManagerSignature;

keyManagementRequest.SerializeToString(&serializedKeyManagementRequest);

//Create socket to talk with caster
std::unique_ptr<zmq::socket_t> keyManagementSocket;

SOM_TRY //Init socket
keyManagementSocket.reset(new zmq::socket_t(*context, ZMQ_REQ));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
keyManagementSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
std::string connectionString = "tcp://127.0.0.1:" +std::to_string(keyManagementPort);
keyManagementSocket->connect(connectionString.c_str());
SOM_CATCH("Error connecting socket for key management request to caster\n")

SOM_TRY //Send key management request
keyManagementSocket->send(serializedKeyManagementRequest.c_str(), serializedKeyManagementRequest.size());
SOM_CATCH("Error sending key management request\n")

std::unique_ptr<zmq::message_t> messageBuffer;

SOM_TRY
messageBuffer.reset(new zmq::message_t);
SOM_CATCH("Error initializing ZMQ message")

REQUIRE(keyManagementSocket->recv(messageBuffer.get()) == true);

key_management_reply keyManagementReply;

keyManagementReply.ParseFromArray(messageBuffer->data(), messageBuffer->size());
REQUIRE(keyManagementReply.IsInitialized() == true);

REQUIRE(keyManagementReply.request_succeeded() == true);

//Create a connection signing key and sign it with the official signing key/

//Create connection key
unsigned char connectionSigningPublicKeyArray[crypto_sign_PUBLICKEYBYTES];
unsigned char connectionSigningSecretKeyArray[crypto_sign_SECRETKEYBYTES];
crypto_sign_keypair(connectionSigningPublicKeyArray, connectionSigningSecretKeyArray);

std::string connectionSigningPublicKey((const char *) connectionSigningPublicKeyArray, crypto_sign_PUBLICKEYBYTES);
std::string connectionSigningSecretKey((const char *) connectionSigningSecretKeyArray, crypto_sign_SECRETKEYBYTES);

//Construct credentials for basestation
Poco::Timestamp currentTime1;
auto timeValue1 = currentTime1.epochMicroseconds() + 1000000*60; //Times out a minute from now

std::string serializedPermissions;
authorized_permissions permissions;
permissions.set_public_key(connectionSigningPublicKey);
permissions.set_valid_until(timeValue1);
permissions.set_number_of_permitted_base_stations(10);
permissions.SerializeToString(&serializedPermissions);

unsigned char permissionsSignatureArray[crypto_sign_BYTES];
std::string permissionsSignatureString;

crypto_sign_detached(permissionsSignatureArray, nullptr, (const unsigned char *) serializedPermissions.c_str(), serializedPermissions.size(), (const unsigned char *) officialSigningSecretKey.c_str());
permissionsSignatureString = std::string((const char *) permissionsSignatureArray, crypto_sign_BYTES);

signature officialSigningSignature;
officialSigningSignature.set_public_key(officialSigningPublicKey);
officialSigningSignature.set_cryptographic_signature(permissionsSignatureString);

credentials connectionKeyCredentials;
*connectionKeyCredentials.mutable_permissions() = serializedPermissions;
(*connectionKeyCredentials.add_signatures()) = officialSigningSignature;


//Create a basestation using tranceiver
// ZMQ PUB -> casterSender -> caster -> ZMQ SUB



std::unique_ptr<zmq::socket_t> testMessagePublisher;

SOM_TRY //Init socket
testMessagePublisher.reset(new zmq::socket_t(*context, ZMQ_PUB));
SOM_CATCH("Error making socket\n")

std::string ZMQPubSocketAddressString = "tcp://*:10001";

SOM_TRY
testMessagePublisher->bind(ZMQPubSocketAddressString.c_str());
SOM_CATCH("Error binding socket\n")


std::unique_ptr<transceiver> com;

SOM_TRY
com.reset(new transceiver(*context));
SOM_CATCH("Error initializing transceiver\n")

//ZMQ Pub receiver
std::string pubDataReceiverAddress;

SOM_TRY
pubDataReceiverAddress = com->createZMQPubDataReceiver("127.0.0.1:10001");
SOM_CATCH("Error, unable to create data receiver\n")

//Caster data sender
std::string senderURI;
SOM_TRY 
senderURI = com->createPylonGPSV2DataSender(pubDataReceiverAddress, connectionSigningSecretKey, connectionKeyCredentials, "127.0.0.1:" +std::to_string(registrationPort), 1.0, 2.0, RTCM_V3_1, "testBasestation", 3.0);
SOM_CATCH("Error making caster sender\n")

std::this_thread::sleep_for(std::chrono::milliseconds(10));

//basestationInfo->set_latitude(1.0);
//basestationInfo->set_longitude(2.0);
//basestationInfo->set_expected_update_rate(3.0);
//basestationInfo->set_message_format(RTCM_V3_1);
//basestationInfo->set_informal_name("testBasestation");



//Generate an update and listen for it
SECTION( "Send/receive update")
{
//Base station registered, so subscribe and see if we get the next message send
//Create socket to subscribe to caster
std::unique_ptr<zmq::socket_t> subscriberSocket;

SOM_TRY //Init socket
subscriberSocket.reset(new zmq::socket_t(*context, ZMQ_SUB));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
subscriberSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
std::string connectionString = "tcp://127.0.0.1:" +std::to_string(clientPublishingPort);
subscriberSocket->connect(connectionString.c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

SOM_TRY //Set filter to allow any published messages to be received
subscriberSocket->setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
SOM_CATCH("Error setting subscription for socket\n")

//Sleep for a few milliseconds to allow connection to stabilize so no messages are missed
std::this_thread::sleep_for(std::chrono::milliseconds(10));

//Send message to caster and see if we get it back with the subscriber
std::string testString = "This is a test string\n";

SOM_TRY //Send message pub -> zmqDataReceiver -> caster -> sub
testMessagePublisher->send(testString.c_str(), testString.size());
SOM_CATCH("Error publishing\n")


SOM_TRY
messageBuffer.reset(new zmq::message_t);
SOM_CATCH("Error initializing ZMQ message")

REQUIRE(subscriberSocket->recv(messageBuffer.get()) == true);

//Check message format
REQUIRE(messageBuffer->size() == (testString.size() + sizeof(Poco::Int64)*2));

REQUIRE(Poco::ByteOrder::fromNetwork(*((Poco::Int64 *) messageBuffer->data())) == casterID);

REQUIRE(std::string((((const char *) messageBuffer->data()) + sizeof(Poco::Int64)*2), testString.size()) == testString);

} //Send/receive update

SECTION( "Send a query")
{
std::this_thread::sleep_for(std::chrono::milliseconds(10));
//Send a query and see if we get a valid response
std::string serializedClientRequest;
client_query_request clientRequest; //Empty request should return all

clientRequest.SerializeToString(&serializedClientRequest);

std::unique_ptr<zmq::socket_t> clientSocket;

SOM_TRY //Init socket
clientSocket.reset(new zmq::socket_t(*context, ZMQ_REQ));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
clientSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
std::string connectionString = "tcp://127.0.0.1:" +std::to_string(clientRequestPort);
clientSocket->connect(connectionString.c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

SOM_TRY
clientSocket->send(serializedClientRequest.c_str(), serializedClientRequest.size());
SOM_CATCH("Error, unable to send client request\n")

SOM_TRY
messageBuffer.reset(new zmq::message_t);
SOM_CATCH("Error initializing ZMQ message")

REQUIRE(clientSocket->recv(messageBuffer.get()) == true);

client_query_reply queryReply;
queryReply.ParseFromArray(messageBuffer->data(), messageBuffer->size());

REQUIRE(queryReply.IsInitialized() == true);
REQUIRE(queryReply.has_caster_id() == true);
REQUIRE(queryReply.caster_id() == casterID);
REQUIRE(queryReply.has_failure_reason() == false);
REQUIRE(queryReply.base_stations_size() == 1);

auto replyBaseStationInfo = queryReply.base_stations(0);
REQUIRE(replyBaseStationInfo.has_latitude());
REQUIRE(replyBaseStationInfo.latitude() == Approx(1.0));
REQUIRE(replyBaseStationInfo.has_longitude());
REQUIRE(replyBaseStationInfo.longitude() == Approx(2.0));
REQUIRE(replyBaseStationInfo.has_expected_update_rate());
REQUIRE(replyBaseStationInfo.expected_update_rate() == Approx(3.0));
REQUIRE(replyBaseStationInfo.has_message_format());
REQUIRE(replyBaseStationInfo.message_format() == RTCM_V3_1);
REQUIRE(replyBaseStationInfo.has_informal_name());
REQUIRE(replyBaseStationInfo.informal_name() == "testBasestation");
REQUIRE(replyBaseStationInfo.has_base_station_id());

//Store station ID so it can be used for later checks
auto baseStationID = replyBaseStationInfo.base_station_id();

//Do it again with a more complex query

//Let the base station entry age a bit, so we can check uptime
std::this_thread::sleep_for(std::chrono::milliseconds(100));

client_query_request clientRequest0; //Empty request should return all

client_subquery subQuery0; //Shouldn't return any results
subQuery0.add_acceptable_classes(COMMUNITY);

sql_double_condition doubleCondition;
sql_integer_condition integerCondition;
sql_string_condition stringCondition;

client_subquery subQuery1; //Should return 1
subQuery1.add_acceptable_classes(OFFICIAL);
subQuery1.add_acceptable_formats(RTCM_V3_1);

doubleCondition.set_value(0.9); 
doubleCondition.set_relation(GREATER_THAN); 
auto doubleConditionBuffer = subQuery1.add_latitude_condition();
(*doubleConditionBuffer) = doubleCondition;
doubleCondition.set_value(1.1); 
doubleCondition.set_relation(LESS_THAN);
doubleConditionBuffer = subQuery1.add_latitude_condition(); 
(*doubleConditionBuffer) = doubleCondition;



doubleCondition.set_value(1.9); 
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_longitude_condition();
(*doubleConditionBuffer) = doubleCondition;
doubleCondition.set_value(2.1); 
doubleCondition.set_relation(LESS_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_longitude_condition();
(*doubleConditionBuffer) = doubleCondition;


doubleCondition.set_value(.01); //Up longer than .01 seconds
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_uptime_condition();
(*doubleConditionBuffer) = doubleCondition;

doubleCondition.set_value(2.9); 
doubleCondition.set_relation(GREATER_THAN_EQUAL_TO); 
doubleConditionBuffer = subQuery1.add_expected_update_rate_condition();
(*doubleConditionBuffer) = doubleCondition;


stringCondition.set_value("testBasestat%");
stringCondition.set_relation(LIKE);
(*subQuery1.mutable_informal_name_condition()) = stringCondition;

integerCondition.set_value(baseStationID);
integerCondition.set_relation(EQUAL_TO);
auto integerConditionBuffer = subQuery1.add_base_station_id_condition();
(*integerConditionBuffer) = integerCondition;


base_station_radius_subquery circleSubQueryPart;
circleSubQueryPart.set_latitude(1.0);
circleSubQueryPart.set_longitude(2.0);
circleSubQueryPart.set_radius(10);
(*subQuery1.mutable_circular_search_region()) = circleSubQueryPart;

//Add subqueries to request
(*clientRequest.add_subqueries()) = subQuery0;
(*clientRequest.add_subqueries()) = subQuery1;

clientRequest.SerializeToString(&serializedClientRequest);

SOM_TRY
clientSocket->send(serializedClientRequest.c_str(), serializedClientRequest.size());
SOM_CATCH("Error, unable to send client request\n")

SOM_TRY
messageBuffer.reset(new zmq::message_t);
SOM_CATCH("Error initializing ZMQ message")

REQUIRE(clientSocket->recv(messageBuffer.get()) == true);

queryReply.ParseFromArray(messageBuffer->data(), messageBuffer->size());
REQUIRE(queryReply.IsInitialized() == true);
REQUIRE(queryReply.has_caster_id() == true);
REQUIRE(queryReply.caster_id() == casterID);
REQUIRE(queryReply.has_failure_reason() == false);
REQUIRE(queryReply.base_stations_size() == 1);

replyBaseStationInfo = queryReply.base_stations(0);
REQUIRE(replyBaseStationInfo.has_latitude());
REQUIRE(replyBaseStationInfo.latitude() == Approx(1.0));
REQUIRE(replyBaseStationInfo.has_longitude());
REQUIRE(replyBaseStationInfo.longitude() == Approx(2.0));
REQUIRE(replyBaseStationInfo.has_expected_update_rate());
REQUIRE(replyBaseStationInfo.expected_update_rate() == Approx(3.0));
REQUIRE(replyBaseStationInfo.has_message_format());
REQUIRE(replyBaseStationInfo.message_format() == RTCM_V3_1);
REQUIRE(replyBaseStationInfo.has_informal_name());
REQUIRE(replyBaseStationInfo.informal_name() == "testBasestation");
} //Finish query

}//Register an authenticated stream


} //Construct caster to test
}



TEST_CASE( "Test simple proxying", "[test]")
{


SECTION( "Build two casters")
{
//Make ZMQ context
std::unique_ptr<zmq::context_t> context;

SOM_TRY
context.reset(new zmq::context_t);
SOM_CATCH("Error initializing ZMQ context\n")

//Generate keys to use
std::string firstCasterPublicKey;
std::string firstCasterPrivateKey;
std::tie(firstCasterPublicKey, firstCasterPrivateKey) = generateSigningKeys();

//Generate key manager signing key
unsigned char firstKeyManagerPublicKeyArray[crypto_sign_PUBLICKEYBYTES];
unsigned char firstKeyManagerSecretKeyArray[crypto_sign_SECRETKEYBYTES];
crypto_sign_keypair(firstKeyManagerPublicKeyArray, firstKeyManagerSecretKeyArray);

std::string firstKeyManagerPublicKey((const char *) firstKeyManagerPublicKeyArray, crypto_sign_PUBLICKEYBYTES);
std::string firstKeyManagerSecretKey((const char *) firstKeyManagerSecretKeyArray, crypto_sign_SECRETKEYBYTES);

Poco::Int64 firstCasterID = 989;
int firstRegistrationPort = 9010;
int firstClientRequestPort = 9013;
int firstClientPublishingPort = 9014;
int firstProxyPublishingPort = 9015;
int firstStreamStatusNotificationPort = 9016;
int firstKeyManagementPort = 9017;


caster firstCaster(context.get(), firstCasterID, firstRegistrationPort, firstClientRequestPort, firstClientPublishingPort, firstProxyPublishingPort, firstStreamStatusNotificationPort, firstKeyManagementPort, firstCasterPublicKey, firstCasterPrivateKey, firstKeyManagerPublicKey, std::vector<std::string>(0), std::vector<std::string>(0), std::vector<std::string>(0));

//Generate keys to use
std::string secondCasterPublicKey;
std::string secondCasterPrivateKey;
std::tie(secondCasterPublicKey, secondCasterPrivateKey) = generateSigningKeys();

//Generate key manager signing key
unsigned char secondKeyManagerPublicKeyArray[crypto_sign_PUBLICKEYBYTES];
unsigned char secondKeyManagerSecretKeyArray[crypto_sign_SECRETKEYBYTES];
crypto_sign_keypair(secondKeyManagerPublicKeyArray, secondKeyManagerSecretKeyArray);

std::string secondKeyManagerPublicKey((const char *) secondKeyManagerPublicKeyArray, crypto_sign_PUBLICKEYBYTES);
std::string secondKeyManagerSecretKey((const char *) secondKeyManagerSecretKeyArray, crypto_sign_SECRETKEYBYTES);

Poco::Int64 secondCasterID = 989+100;
int secondRegistrationPort = 9010+100;
int secondClientRequestPort = 9013+100;
int secondClientPublishingPort = 9014+100;
int secondProxyPublishingPort = 9015+100;
int secondStreamStatusNotificationPort = 9016+100;
int secondKeyManagementPort = 9017+100;


caster secondCaster(context.get(), secondCasterID, secondRegistrationPort, secondClientRequestPort, secondClientPublishingPort, secondProxyPublishingPort, secondStreamStatusNotificationPort, secondKeyManagementPort, secondCasterPublicKey, secondCasterPrivateKey, secondKeyManagerPublicKey, std::vector<std::string>(0), std::vector<std::string>(0), std::vector<std::string>(0));

SECTION( "Add stream to first caster, proxy second to first, and add second stream to first caster, then send/receive messages")
{
//Add first unauthenticated stream
//Create a basestation and register it

//Create socket to talk with caster
std::unique_ptr<zmq::socket_t> firstRegistrationSocket;

SOM_TRY //Init socket
firstRegistrationSocket.reset(new zmq::socket_t(*context, ZMQ_DEALER));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
firstRegistrationSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
std::string connectionString = "tcp://127.0.0.1:" +std::to_string(firstRegistrationPort);
firstRegistrationSocket->connect(connectionString.c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

transmitter_registration_request firstRegistrationRequest;
auto basestationInfo = firstRegistrationRequest.mutable_stream_info();
basestationInfo->set_latitude(1.0);
basestationInfo->set_longitude(2.0);
basestationInfo->set_expected_update_rate(3.0);
basestationInfo->set_message_format(RTCM_V3_1);
basestationInfo->set_informal_name("testBasestation");

transmitter_registration_reply firstRegistrationReply;

bool messageReceived = false;
bool messageDeserializedCorrectly = false;
SOM_TRY
std::tie(messageReceived, messageDeserializedCorrectly) = remoteProcedureCall(*firstRegistrationSocket, firstRegistrationRequest, firstRegistrationReply);
SOM_CATCH("Error, stream registration failed\n")

REQUIRE(firstRegistrationReply.request_succeeded() == true);

//Registration of first stream completed, so make second caster proxy of first caster
std::string addProxyClientRequestConnectionString = "tcp://127.0.0.1:" + std::to_string(firstClientRequestPort);
std::string addProxyBasestationPublishingConnectionString = "tcp://127.0.0.1:" + std::to_string(firstProxyPublishingPort);
std::string addProxyConnectDisconnectNotificationConnectionString = "tcp://127.0.0.1:" + std::to_string(firstStreamStatusNotificationPort);

SOM_TRY
secondCaster.addProxy(addProxyClientRequestConnectionString, addProxyBasestationPublishingConnectionString, addProxyConnectDisconnectNotificationConnectionString);
SOM_CATCH("Error, unable to add proxy\n")

//Create socket so that the state of the second caster can be determined
std::unique_ptr<zmq::socket_t> clientSocket;

SOM_TRY //Init socket
clientSocket.reset(new zmq::socket_t(*context, ZMQ_REQ));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
clientSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
std::string connectionString = "tcp://127.0.0.1:" +std::to_string(secondClientRequestPort);
clientSocket->connect(connectionString.c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

int64_t firstBaseStationID = 0;

//Give a little time for the updates to propogate
std::this_thread::sleep_for(std::chrono::milliseconds(10));

//Send a query and verify the added basestation shows up in the second caster
client_query_request queryRequest; //Empty request should return all
client_query_reply queryReply;

SOM_TRY
std::tie(messageReceived, messageDeserializedCorrectly) = remoteProcedureCall(*clientSocket, queryRequest, queryReply);
SOM_CATCH("Error, stream registration failed\n")


REQUIRE(messageReceived == true);
REQUIRE(messageDeserializedCorrectly == true);
REQUIRE(queryReply.has_caster_id() == true);
REQUIRE(queryReply.caster_id() == secondCaster.casterID);
REQUIRE(queryReply.has_failure_reason() == false);
REQUIRE(queryReply.base_stations_size() == 1);

auto replyBaseStationInfo = queryReply.base_stations(0);
REQUIRE(replyBaseStationInfo.has_latitude());
REQUIRE(replyBaseStationInfo.latitude() == Approx(1.0));
REQUIRE(replyBaseStationInfo.has_longitude());
REQUIRE(replyBaseStationInfo.longitude() == Approx(2.0));
REQUIRE(replyBaseStationInfo.has_expected_update_rate());
REQUIRE(replyBaseStationInfo.expected_update_rate() == Approx(3.0));
REQUIRE(replyBaseStationInfo.has_message_format());
REQUIRE(replyBaseStationInfo.message_format() == RTCM_V3_1);
REQUIRE(replyBaseStationInfo.has_informal_name());
REQUIRE(replyBaseStationInfo.informal_name() == "testBasestation");
REQUIRE(replyBaseStationInfo.has_base_station_id());

//Store station ID so it can be used for later checks
firstBaseStationID = replyBaseStationInfo.base_station_id();

//Add a second basestation to the first caster
//Create socket to talk with caster
std::unique_ptr<zmq::socket_t> secondRegistrationSocket;

SOM_TRY //Init socket
secondRegistrationSocket.reset(new zmq::socket_t(*context, ZMQ_DEALER));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
secondRegistrationSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
std::string connectionString = "tcp://127.0.0.1:" +std::to_string(firstRegistrationPort);
secondRegistrationSocket->connect(connectionString.c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

transmitter_registration_request secondRegistrationRequest;
auto basestationInfo0 = secondRegistrationRequest.mutable_stream_info();
basestationInfo0->set_latitude(4.0);
basestationInfo0->set_longitude(5.0);
basestationInfo0->set_expected_update_rate(6.0);
basestationInfo0->set_message_format(RINEX);
basestationInfo0->set_informal_name("testBasestation2");

transmitter_registration_reply secondRegistrationReply;

messageReceived = false;
messageDeserializedCorrectly = false;
SOM_TRY
std::tie(messageReceived, messageDeserializedCorrectly) = remoteProcedureCall(*secondRegistrationSocket, secondRegistrationRequest, secondRegistrationReply);
SOM_CATCH("Error, stream registration failed\n")

REQUIRE(messageReceived == true);
REQUIRE(messageDeserializedCorrectly == true);
REQUIRE(secondRegistrationReply.request_succeeded() == true);

//Send a query and verify that both of the basestations show up in the second caster
client_query_request queryRequest2; //Empty request should return all
client_query_reply queryReply2;

//Give a little time for updates to propagate
std::this_thread::sleep_for(std::chrono::milliseconds(10));

SOM_TRY
std::tie(messageReceived, messageDeserializedCorrectly) = remoteProcedureCall(*clientSocket, queryRequest2, queryReply2);
SOM_CATCH("Error, stream registration failed\n")


REQUIRE(messageReceived == true);
REQUIRE(messageDeserializedCorrectly == true);
REQUIRE(queryReply2.has_caster_id() == true);
REQUIRE(queryReply2.caster_id() == secondCaster.casterID);
REQUIRE(queryReply2.has_failure_reason() == false);
REQUIRE(queryReply2.base_stations_size() == 2);

int64_t secondBaseStationID = 0;

for(int i=0; i<2; i++)
{
auto replyBaseStationInfo = queryReply2.base_stations(i);
REQUIRE(replyBaseStationInfo.has_base_station_id());

if(replyBaseStationInfo.base_station_id() == firstBaseStationID)
{ //Check the first basestation details
REQUIRE(replyBaseStationInfo.has_latitude());
REQUIRE(replyBaseStationInfo.latitude() == Approx(1.0));
REQUIRE(replyBaseStationInfo.has_longitude());
REQUIRE(replyBaseStationInfo.longitude() == Approx(2.0));
REQUIRE(replyBaseStationInfo.has_expected_update_rate());
REQUIRE(replyBaseStationInfo.expected_update_rate() == Approx(3.0));
REQUIRE(replyBaseStationInfo.has_message_format());
REQUIRE(replyBaseStationInfo.message_format() == RTCM_V3_1);
REQUIRE(replyBaseStationInfo.has_informal_name());
REQUIRE(replyBaseStationInfo.informal_name() == "testBasestation");
}
else
{ //Check second basestation details
REQUIRE(replyBaseStationInfo.has_latitude());
REQUIRE(replyBaseStationInfo.latitude() == Approx(4.0));
REQUIRE(replyBaseStationInfo.has_longitude());
REQUIRE(replyBaseStationInfo.longitude() == Approx(5.0));
REQUIRE(replyBaseStationInfo.has_expected_update_rate());
REQUIRE(replyBaseStationInfo.expected_update_rate() == Approx(6.0));
REQUIRE(replyBaseStationInfo.has_message_format());
REQUIRE(replyBaseStationInfo.message_format() == RINEX);
REQUIRE(replyBaseStationInfo.has_informal_name());
REQUIRE(replyBaseStationInfo.informal_name() == "testBasestation2");
secondBaseStationID = replyBaseStationInfo.base_station_id();
}
} //End for

//Send updates and see if they show up at the proxy
//Create socket to subscribe to the second caster
std::unique_ptr<zmq::socket_t> subscriberSocket;

SOM_TRY //Init socket
subscriberSocket.reset(new zmq::socket_t(*context, ZMQ_SUB));
SOM_CATCH("Error making socket\n")

SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
subscriberSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
std::string connectionString = "tcp://127.0.0.1:" +std::to_string(secondClientPublishingPort);
subscriberSocket->connect(connectionString.c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

SOM_TRY //Set filter to allow any published messages to be received
subscriberSocket->setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
SOM_CATCH("Error setting subscription for socket\n")

//Sleep for a few milliseconds to allow connection to stabilize so no messages are missed
std::this_thread::sleep_for(std::chrono::milliseconds(10));

//Send a message via each of the basestations
std::string firstSocketUpdateString = "Hello world\n";
std::string secondSocketUpdateString = "Sharing is caring\n";

SOM_TRY
firstRegistrationSocket->send(firstSocketUpdateString.c_str(), firstSocketUpdateString.size());
SOM_CATCH("Error sending update\n")

SOM_TRY
secondRegistrationSocket->send(secondSocketUpdateString.c_str(), secondSocketUpdateString.size());
SOM_CATCH("Error sending update\n")

for(int i=0; i<2; i++)
{
zmq::message_t updateMessageBuffer;

SOM_TRY
REQUIRE(subscriberSocket->recv(&updateMessageBuffer) == true);
SOM_CATCH("Error receiving updates from proxy\n")

REQUIRE(updateMessageBuffer.size() > sizeof(Poco::Int64)*2);

int64_t casterID = Poco::ByteOrder::fromNetwork(((Poco::Int64*) updateMessageBuffer.data())[0]);

int64_t streamID = Poco::ByteOrder::fromNetwork(((Poco::Int64*) updateMessageBuffer.data())[1]);

REQUIRE(casterID == secondCaster.casterID);

std::string receivedUpdateContent = std::string( ((const char *) updateMessageBuffer.data())+sizeof(Poco::Int64)*2, updateMessageBuffer.size() - sizeof(Poco::Int64)*2);

if(streamID == firstBaseStationID)
{
REQUIRE(receivedUpdateContent == firstSocketUpdateString);
}
else if(streamID == secondBaseStationID)
{
REQUIRE(receivedUpdateContent == secondSocketUpdateString);
}


}


} //Add and proxy
} //Construct casters
}

TEST_CASE( "Test tranceiver", "[test]")
{
SECTION( "Ping pong updates across many different receivers/senders")
{
//Make ZMQ context
std::unique_ptr<zmq::context_t> context;

SOM_TRY
context.reset(new zmq::context_t);
SOM_CATCH("Error initializing ZMQ context\n")

std::unique_ptr<transceiver> com;

SOM_TRY
com.reset(new transceiver(*context));
SOM_CATCH("Error initializing transceiver\n")

std::unique_ptr<zmq::socket_t> messagePublishingSocket;

SOM_TRY
messagePublishingSocket.reset(new zmq::socket_t(*context, ZMQ_PUB));
SOM_CATCH("Error creating socket\n")

std::string pubSocketAddressString = "inproc://pubSocketAddress";

SOM_TRY
messagePublishingSocket->bind(pubSocketAddressString.c_str());
SOM_CATCH("Error binding socket\n")

//Create sender/receiver chain
//zmq pub sender -> zmq pub receiver -> file data sender -> file data reciever -> tcp data sender -> tcp data receiver 

std::string receiverAddress;
std::string senderURI;

SOM_TRY
com->createZMQDataSender(pubSocketAddressString, 9001);
SOM_CATCH("Error creating sender\n")

SOM_TRY
receiverAddress = "";
receiverAddress = com->createZMQPubDataReceiver("127.0.0.1:9001");
SOM_CATCH("Error creating sender\n")

std::string tempFilePath = "tempFile902e809843";

//Remove temp file if it is left over
remove(tempFilePath.c_str());


SOM_TRY
com->createFileDataSender(receiverAddress, tempFilePath);
SOM_CATCH("Error creating sender\n")

SOM_TRY
receiverAddress = "";
receiverAddress = com->createFileDataReceiver(tempFilePath);
SOM_CATCH("Error creating sender\n")



SOM_TRY
com->createTCPDataSender(receiverAddress, 9002);
SOM_CATCH("Error creating sender\n")


SOM_TRY
receiverAddress = "";
receiverAddress = com->createTCPDataReceiver("127.0.0.1:9002");
SOM_CATCH("Error creating sender\n")



std::unique_ptr<zmq::socket_t> subscriberSocket;

SOM_TRY //Init socket
subscriberSocket.reset(new zmq::socket_t(*context, ZMQ_SUB));
SOM_CATCH("Error making socket\n")

//Subscribe to the results from the last publisher
SOM_TRY
int timeoutWaitTime = 5000; //Max 5 seconds
subscriberSocket->setsockopt(ZMQ_RCVTIMEO, (void *) &timeoutWaitTime, sizeof(timeoutWaitTime));
SOM_CATCH("Error setting socket timeout\n")

SOM_TRY //Connect to caster
subscriberSocket->connect(receiverAddress.c_str());
SOM_CATCH("Error connecting socket for registration with caster\n")

SOM_TRY //Set filter to allow any published messages to be received
subscriberSocket->setsockopt(ZMQ_SUBSCRIBE, nullptr, 0);
SOM_CATCH("Error setting subscription for socket\n")

std::string testMessage = "C";


//Sleep for a little while to let subscribers connect so messages aren't dropped
std::this_thread::sleep_for(std::chrono::milliseconds(10));

//Send test message through chain
SOM_TRY
messagePublishingSocket->send(testMessage.c_str(), testMessage.size());
SOM_CATCH("Error sending test message\n")

zmq::message_t messageBuffer;

SOM_TRY
REQUIRE(subscriberSocket->recv(&messageBuffer) == true);
SOM_CATCH("Error receiving test message\n")

REQUIRE(messageBuffer.size() == testMessage.size());

REQUIRE(((char *) messageBuffer.data())[0] == testMessage[0]);

//Remove temp file
remove(tempFilePath.c_str());

}

}
*/

