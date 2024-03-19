import 'dart:async';
import 'dart:convert';
import 'dart:io';
import 'dart:math';
import 'package:file_picker/file_picker.dart';
import 'package:get/get.dart';
import 'package:get/get_core/src/get_main.dart';
import 'package:readair/BLE/ble_setup.dart';
import 'package:readair/data/packet.dart';
import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';
import 'package:collection/collection.dart';
import 'package:flutter/services.dart' show Uint8List, rootBundle;

class DeviceDetailsPage extends StatefulWidget {
  final BluetoothDevice device;
  final VoidCallback onDisconnect;

  const DeviceDetailsPage({
    Key? key,
    required this.device,
    required this.onDisconnect,
  }) : super(key: key);

  @override
  State<DeviceDetailsPage> createState() => _DeviceDetailsPageState();
}

class _DeviceDetailsPageState extends State<DeviceDetailsPage> {
  List<BluetoothService> _services = [];
  Timer? _timer;
  String? _latestReceivedPacket;
  bool isSubscribed = false;
  int _connectionStep = 0;
  BluetoothCharacteristic? writeCharacteristic;
  BluetoothCharacteristic? readCharacteristic;

  @override
  void initState() {
    super.initState();
    _connectAndSetupDevice();
  }

  Future<void> _connectAndSetupDevice() async {
    // Ensure services are discovered
    await _discoverServices();

    // Request MTU change
    await _requestMtuSize(512);

    final BluetoothController bluetoothController =
        Get.find<BluetoothController>();

    // Subscribe to the device
    bluetoothController.subscribeToDevice(widget.device);
  }

  @override
  void dispose() {
    _timer?.cancel();
    isSubscribed = false;

    super.dispose();
  }

  //--------------------------------INITIALIZATION----------------------------------------------

  Future<void> _discoverServices() async {
    _services = await widget.device.discoverServices();
    for (BluetoothService service in _services) {
      var characteristics = service.characteristics;
      for (BluetoothCharacteristic characteristic in characteristics) {
        if (characteristic.uuid ==
            Guid("588d30b0-33aa-4654-ab36-56dfa9974b13")) {
          writeCharacteristic = characteristic;
          print("Write characteristic found");
          break;
        }
        if (characteristic.uuid ==
            Guid("588d30b0-33aa-4654-ab36-56dfa9974b13")) {
          readCharacteristic = characteristic;
          print("Read characteristic found");
        }
      }
    }
  }

  Future<void> _initializeEsp32Connection() async {
    // Send current time to ESP32
    await _sendTimeToEsp32();
    await Future.delayed(Duration(seconds: 1));

    // Send 'TGMT=-7' command
    await _sendData('TGMT=-7');
    await Future.delayed(Duration(seconds: 1));

    // Subscribe to ESP32
    await _subscribeToDevice();
  }

  Future<void> _initializeAndAutoRead() async {
    await _initializeEsp32Connection();
    await Future.delayed(Duration(seconds: 3));
    await _sendData("READ!");
    await Future.delayed(Duration(seconds: 7));

    _setupNotificationForImmediateRead();
  }

  Future<void> _requestMtuSize(int requestedMtu) async {
    try {
      int resultingMtu = await widget.device.requestMtu(requestedMtu);
      print('MTU size after negotiation: $resultingMtu');
    } catch (e) {
      print('Error requesting MTU size: $e');
    }
  }

//---------------------------------SUBSCRIPTION--------------------------------------------------

  Future<void> _subscribeToDevice() async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    final targetService =
        _services.firstWhereOrNull((s) => s.uuid == serviceUuid);

    if (targetService != null) {
      var characteristic = targetService.characteristics
          .firstWhereOrNull((c) => c.uuid == characteristicUuid);

      if (characteristic != null) {
        await characteristic.setNotifyValue(true);
        characteristic.value.listen((value) {
          String receivedData = String.fromCharCodes(value);
          _latestReceivedPacket = receivedData;
          _processDataPacket(receivedData);
          //_showNotification("Data received from ESP32");
        });

        setState(() {
          isSubscribed = true;
        });
      }
    }
  }

  void _setupNotificationForImmediateRead() async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    final targetService =
        _services.firstWhereOrNull((s) => s.uuid == serviceUuid);

    if (targetService != null) {
      var characteristic = targetService.characteristics
          .firstWhereOrNull((c) => c.uuid == characteristicUuid);

      if (characteristic != null) {
        await characteristic.setNotifyValue(true);
        characteristic.value.listen((value) {
          // As soon as data is received, process and read the next packet
          String receivedData = String.fromCharCodes(value);
          _latestReceivedPacket = receivedData;
          _processDataPacket(receivedData);

          // Immediately trigger the read data function
          _receiveAndReadData();
        });
        setState(() {
          isSubscribed = true;
        });
      }
    }
  }

//--------------------------------READ/WRITE-------------------------------------------------------

  Future<void> _sendTimeToEsp32() async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    // Get the current epoch time in seconds.
    int epochTime = DateTime.now().millisecondsSinceEpoch ~/ 1000;

    // Create the command string.
    String timeCommand = "TIME=$epochTime";

    // Convert the command string to bytes.
    List<int> bytesToSend = utf8.encode(timeCommand);

    // Find the service.
    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == serviceUuid,
    );

    if (targetService != null) {
      // Find the characteristic.
      var characteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == characteristicUuid,
      );

      if (characteristic != null) {
        // Write the time command to the ESP32.
        await characteristic.write(bytesToSend, withoutResponse: false);
      }
    }
  }

  void _rapidReadCommand() async {
    _sendData("READ!");
    await Future.delayed(Duration(seconds: 3));

    const rapidReadInterval = Duration(
        milliseconds: 50); // Adjust this interval as needed for faster reading
    _timer?.cancel();

    _timer = Timer.periodic(rapidReadInterval, (Timer t) async {
      await _readDataFromEsp32();
    });
  }

  Future<void> _readDataFromEsp32() async {
    if (writeCharacteristic != null) {
      try {
        var value = await writeCharacteristic!.read();
        String receivedData = String.fromCharCodes(value);
        _latestReceivedPacket = receivedData;
        _processDataPacket(receivedData);
        print("Data read successfully");
      } catch (e) {
        print("Error reading from ESP32: $e");
      }
    } else {
      print("Read characteristic not found");
    }
  }

  Future<void> _autoReads() async {
    await _sendData("READ!");
    await Future.delayed(Duration(seconds: 3));

    _setupNotificationForImmediateRead();
  }

  Future<void> _receiveAndReadData() async {
    final writeCharacteristicUuid =
        Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");
    final readCharacteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");

    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == serviceUuid,
    );

    if (targetService != null) {
      var writeCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == writeCharacteristicUuid,
      );

      var readCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == readCharacteristicUuid,
      );

      if (writeCharacteristic != null && readCharacteristic != null) {
        // Read the response
        await readCharacteristic.read().then((value) {
          String receivedData = String.fromCharCodes(value);
          //_showMessage(receivedData);
          _latestReceivedPacket = receivedData;
          _processDataPacket(receivedData);
        });
      }
    }
  }

  Future<void> _sendAndReadData() async {
    final writeCharacteristicUuid =
        Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");
    final readCharacteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");

    // Convert the String "READ!" to a list of bytes
    List<int> bytesToSend = utf8.encode("READ!");

    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == serviceUuid,
    );

    if (targetService != null) {
      var writeCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == writeCharacteristicUuid,
      );

      var readCharacteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == readCharacteristicUuid,
      );

      if (writeCharacteristic != null && readCharacteristic != null) {
        // Write "READ!" to the ESP32
        await writeCharacteristic.write(bytesToSend, withoutResponse: false);
        // Read the response
        await readCharacteristic.read().then((value) {
          String receivedData = String.fromCharCodes(value);
          _showReceivedDataDialog(receivedData);
        });
      }
    }
  }

  Future<void> _sendData(String dataToSend) async {
    final serviceUuid = Guid("9194f647-3a6c-4cf2-a6d5-187cb05728cd");
    final characteristicUuid = Guid("588d30b0-33aa-4654-ab36-56dfa9974b13");

    // Convert the String to a list of bytes
    List<int> bytesToSend = utf8.encode(dataToSend);

    final targetService = _services.firstWhereOrNull(
      (s) => s.uuid == serviceUuid,
    );

    if (targetService != null) {
      var characteristic = targetService.characteristics.firstWhereOrNull(
        (c) => c.uuid == characteristicUuid,
      );

      if (characteristic != null) {
        await characteristic.write(bytesToSend, withoutResponse: false);
      }
    }
  }

  Future<void> _readFromEspAndDisplay() async {
    try {
      // Read the characteristic
      var value = await readCharacteristic?.read();
      String receivedData = String.fromCharCodes(value ?? []);

      // Display the result in an alert dialog
      _showReceivedDataDialog(receivedData);
    } catch (e) {
      print("Error reading from ESP32: $e");
      _showMessage("Error reading from ESP32: $e");
    }
  }

//--------------------------------PARSING----------------------------------------------------------

  void _processDataPacket(String data) {
    // Split the incoming data by new lines to handle multiple packets
    var packets = data.trim().split('\n');
    for (var packetData in packets) {
      try {
        var parsedData = packetData.split(',');
        if ((data.length == 12) || (data.length == 13)) {
          var packet = DataPacket(
            epochTime: double.tryParse(parsedData[0]) ?? 0.0,
            co2: double.tryParse(parsedData[1]) ?? 0.0,
            ppm1_0: double.tryParse(parsedData[2]) ?? 0.0,
            ppm2_5: double.tryParse(parsedData[3]) ?? 0.0,
            ppm4_0: double.tryParse(parsedData[4]) ?? 0.0,
            ppm10_0: double.tryParse(parsedData[5]) ?? 0.0,
            humid: double.tryParse(parsedData[6]) ?? 0.0,
            temp: double.tryParse(parsedData[7]) ?? 0.0,
            voc: double.tryParse(parsedData[8]) ?? 0.0,
            nox: double.tryParse(parsedData[9]) ?? 0.0,
            co: double.tryParse(parsedData[10]) ?? 0.0,
            ng: double.tryParse(parsedData[11]) ?? 0.0,
            aqi: double.tryParse(parsedData[12]) ?? 0.0,
          );

          print(packetData);
          DatabaseService.instance.insertOrUpdateDataPacket(packet);
        } else {
          _showMessage("Received data does not match expected format.");
        }
      } catch (e) {
        print("Error processing packet: $e");
      }
    }
  }

  Future<void> _readLatestPacket() async {
    var packet = await DatabaseService.instance.getLastPacket();
    if (packet != null) {
      _showReceivedDataDialog(packet.toString());
    } else {
      _showReceivedDataDialog("No data available");
    }
  }

//--------------------------------MESSAGES--------------------------------------------------------

  void _showNotification(String message) {
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(
      content: Text(message),
      duration: Duration(seconds: 2),
    ));
  }

  void _showMessage(String message) {
    ScaffoldMessenger.of(context)
        .showSnackBar(SnackBar(content: Text(message)));
  }

  void _showReceivedDataDialog(String data) {
    showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: Text('Received Data'),
          content: Text(data),
          actions: <Widget>[
            TextButton(
              child: Text('Close'),
              onPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );
  }

//--------------------------------OTA UPDATES------------------------------------------------------

  Future<List<int>> loadBinFile() async {
    final byteData = await rootBundle.load('assets/update1.bin');
    return byteData.buffer.asUint8List();
  }

  Future<void> sendFirmwareUpdate() async {
    try {
      await _sendData('KAZAM');
      print("KAZAM sent");

      await Future.delayed(Duration(seconds: 2));

      await _waitForAck();
      print("ACK received");

      await Future.delayed(Duration(seconds: 2));

      List<int> fileBytes = await loadBinFile();
      int fileSize = fileBytes.length;

      //Send the size of the update binary file
      await _sendData("SIZE=${fileSize.toString()}");

      await Future.delayed(Duration(seconds: 1));

      await _sendFileInChunks(fileBytes, 509);
    } catch (e) {
      _showMessage("Error during firmware update: $e");
      print("Error during firmware update: $e");
    }
  }
//

  Future<void> _sendFileInChunks(List<int> fileBytes, int chunkSize) async {
    int bytesTransferred = 0;
    for (int i = 0; i * chunkSize < fileBytes.length; i++) {
      int start = i * chunkSize;
      int end = min(fileBytes.length, (i + 1) * chunkSize);
      List<int> chunk = fileBytes.sublist(start, end);

      await writeCharacteristic!.write(chunk, withoutResponse: false);
      bytesTransferred += chunk.length;

      //print(chunk);

      double progress = (bytesTransferred / fileBytes.length) * 100;
      if (progress >= 5 && progress % 5 < 0.1) {
        _showMessage("${progress.toInt()}% uploaded.");
      }

      //await Future.delayed(Duration(milliseconds: 20));
    }
    _showMessage("File upload complete.");
    _sendData("DONE!");
  }

  Future<void> _waitForAck(
      {Duration timeout = const Duration(seconds: 10)}) async {
    // Check if the characteristic is set
    if (readCharacteristic == null) {
      print("Read characteristic is not set.");
      return;
    }

    // Use a Completer to wait for the 'a' character
    Completer<void> ackCompleter = Completer();

    // Subscribe to characteristic changes
    final subscription = writeCharacteristic!.value.listen((value) {
      String receivedData = String.fromCharCodes(value);
      print("Received Data: $receivedData");

      // Complete the completer if 'a' is received
      if (receivedData.contains('a')) {
        if (!ackCompleter.isCompleted) {
          ackCompleter.complete();
        }
      }
    });

    // Wait for either the 'a' character or the timeout
    try {
      await Future.any([
        ackCompleter.future,
        Future.delayed(timeout, () {
          // If the timeout completes first, throw a TimeoutException
          if (!ackCompleter.isCompleted) {
            ackCompleter.completeError(TimeoutException(
                "Did not receive 'a' in the allotted time: $timeout"));
          }
        })
      ]);
      print("Received 'a'. Acknowledgement complete.");
    } catch (e) {
      print(e); // Rethrow the exception to be handled by the caller
    } finally {
      await subscription.cancel();
    }
  }

  Future<bool> checkBinFileAvailability() async {
    try {
      final byteData = await rootBundle.load('assets/HomeAir.ino.bin');
      print("File is available");
      return true;
    } catch (e) {
      print("Error: File not available - $e");
      return false;
    }
  }

  Widget build(BuildContext context) {
    final BluetoothController controller = Get.find<BluetoothController>();
    return Scaffold(
      appBar: AppBar(
        title: const Text('Device Details'),
      ),
      body: Padding(
        padding: EdgeInsets.all(16.0),
        child: Column(
          crossAxisAlignment: CrossAxisAlignment.start,
          children: [
            Text('Device Name: ${widget.device.name ?? 'Unknown'}',
                style: TextStyle(fontSize: 20)),
            SizedBox(height: 8),
            Text('Device ID: ${widget.device.id.id}',
                style: TextStyle(fontSize: 20)),
            SizedBox(height: 24),
            Obx(() => Text(
                  controller.isSubscribed.value
                      ? "Subscribed"
                      : "Not Subscribed",
                  style: TextStyle(
                    fontSize: 20,
                    color: controller.isSubscribed.value
                        ? Colors.green
                        : Colors.red,
                  ),
                )),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _initializeEsp32Connection,
                child: Text('Initialize ESP32 Connection'),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _initializeAndAutoRead,
                child: Text('Initialize then start reading'),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _autoReads,
                child: Text('Start Reading'),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _rapidReadCommand,
                child: Text('Read Every 100 miliseconds'),
              ),
            ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: () {
            //       _sendData("UPDAT");
            //     },
            //     child: Text('Send UPDAT'),
            //   ),
            // ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: () {
            //       _sendData("STAT!");
            //     },
            //     child: Text('Send STAT'),
            //   ),
            // ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: sendFirmwareUpdate,
                child: Text('Send Bin'),
              ),
            ),

            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: () async {
                  bool isFileAvailable = await checkBinFileAvailability();
                  String message = isFileAvailable
                      ? "Bin file is available."
                      : "Bin file is not available.";
                  _showMessage(message);
                },
                child: Text('Check Bin File Availability'),
              ),
            ),
            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: _readFromEspAndDisplay,
                child: Text('Read Data from ESP'),
              ),
            ),

            Padding(
              padding: const EdgeInsets.all(8.0),
              child: ElevatedButton(
                onPressed: widget.onDisconnect,
                child: Text('Disconnect'),
              ),
            ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _addRandomPacket,
            //     child: Text('Add Random Packet'),
            //   ),
            // ),
            // Padding(
            //   padding: const EdgeInsets.all(8.0),
            //   child: ElevatedButton(
            //     onPressed: _readLatestPacket,
            //     child: Text('Read Latest Packet'),
            //   ),
            // ),
          ],
        ),
      ),
    );
  }
}
