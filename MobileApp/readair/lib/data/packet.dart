import 'package:sqflite/sqflite.dart';
import 'package:path/path.dart';

// Model class for a data packet
class DataPacket {
  final double epochTime;
  final double co2;
  final double ppm1_0;
  final double ppm2_5;
  final double ppm4_0;
  final double ppm10_0;
  final double humid;
  final double temp;
  final double voc;
  final double co;
  final double ng;
  final double aqi;

  DataPacket({
    required this.epochTime,
    required this.co2,
    required this.ppm1_0,
    required this.ppm2_5,
    required this.ppm4_0,
    required this.ppm10_0,
    required this.humid,
    required this.temp,
    required this.voc,
    required this.co,
    required this.ng,
    required this.aqi,
  });

  Map<String, double> toMap() {
    return {
      'epochTime': epochTime,
      'co2': co2,
      'ppm1_0': ppm1_0,
      'ppm2_5': ppm2_5,
      'ppm4_0': ppm4_0,
      'ppm10_0': ppm10_0,
      'humidity': humid,
      'temperature': temp,
      'voc': voc,
      'co': co,
      'ng': ng,
      'aqi': aqi,
    };
  }

    List<dynamic> toList() {
    return [
      epochTime,
      co2,
      ppm1_0,
      ppm2_5,
      ppm4_0,
      ppm10_0,
      humid,
      temp,
      voc,
      co,
      ng,
      aqi
    ];
  }

  factory DataPacket.fromMap(Map<String, dynamic> map) {
    return DataPacket(
      epochTime: map['epochTime'],
      co2: map['co2'],
      ppm1_0: map['ppm1_0'],
      ppm2_5: map['ppm2_5'],
      ppm4_0: map['ppm4_0'],
      ppm10_0: map['ppm10_0'],
      humid: map['humidity'],
      temp: map['temperature'],
      voc: map['voc'],
      co: map['co'],
      ng: map['ng'],
      aqi: map['aqi'],
    );
  }

  @override
  String toString() {
    return 'DataPacket{'
           'epochTime: ${epochTime.toStringAsFixed(1)}, '
           'co2: ${co2.toStringAsFixed(1)}, '
           'ppm1_0: ${ppm1_0.toStringAsFixed(1)}, '
           'ppm2_5: ${ppm2_5.toStringAsFixed(1)}, '
           'ppm4_0: ${ppm4_0.toStringAsFixed(1)}, '
           'ppm10_0: ${ppm10_0.toStringAsFixed(1)}, '
           'humid: ${humid.toStringAsFixed(1)}, '
           'temp: ${temp.toStringAsFixed(1)}, '
           'voc: ${voc.toStringAsFixed(1)}, '
           'co: ${co.toStringAsFixed(1)}, '
           'ng: ${ng.toStringAsFixed(1)}, '
           'aqi: ${aqi.toStringAsFixed(1)}}';
  }
}

class DatabaseService {
  static final DatabaseService instance = DatabaseService._init();
  static Database? _database;

  DatabaseService._init();

  Future<Database> get database async {
    if (_database != null) return _database!;
    _database = await _initialize();
    return _database!;
  }

  Future<Database> _initialize() async {
    final path = await getFullPath();
    return await openDatabase(path, version: 1, onCreate: _createDB);
  }

  Future<String> getFullPath() async {
    const name = 'measurement_database.db';
    final path = await getDatabasesPath();
    return join(path, name);
  }

  Future<void> deleteAllPackets() async {
  final db = await database;
  await db.delete('data_packets');
}

  Future<List<DataPacket>> getLastFivePackets() async {
    final db = await database;
    final List<Map<String, dynamic>> maps = await db.query(
      'data_packets',
      orderBy: 'epochTime DESC',
      limit: 100,
    );

    return List.generate(maps.length, (i) {
      return DataPacket.fromMap(maps[i]);
    });
  }

  Future _createDB(Database db, int version) async {
    await db.execute('''
    CREATE TABLE data_packets(
      epochTime REAL PRIMARY KEY,
      co2 REAL,
      ppm1_0 REAL,
      ppm2_5 REAL,
      ppm4_0 REAL,
      ppm10_0 REAL,
      humidity REAL,
      temperature REAL,
      voc REAL,
      co REAL,
      ng REAL,
      aqi REAL
    )
  ''');
  }

  Future<void> insertOrUpdateDataPacket(DataPacket packet) async {
    final db = await instance.database;
    await db.execute('''
      INSERT INTO data_packets 
      (epochTime, co2, ppm1_0, ppm2_5, ppm4_0, ppm10_0, humidity, temperature, voc, co, ng, aqi) 
      VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
      ON CONFLICT(epochTime) 
      DO UPDATE SET 
      co2 = excluded.co2, 
      ppm1_0 = excluded.ppm1_0,
      ppm2_5 = excluded.ppm2_5,
      ppm4_0 = excluded.ppm4_0,
      ppm10_0 = excluded.ppm10_0,
      humidity = excluded.humidity,
      temperature = excluded.temperature,
      voc = excluded.voc,
      co = excluded.co,
      ng = excluded.ng,
      aqi = excluded.aqi
    ''', [
      packet.epochTime,
      packet.co2,
      packet.ppm1_0,
      packet.ppm2_5,
      packet.ppm4_0,
      packet.ppm10_0,
      packet.humid,
      packet.temp,
      packet.voc,
      packet.co,
      packet.ng,
      packet.aqi,
    ]);
  }
  
  Future<List<DataPacket>> getAllPackets() async {
    final db = await database;
    final List<Map<String, dynamic>> maps = await db.query('data_packets');

    return maps.map((map) => DataPacket.fromMap(map)).toList();
  }
  
  Future<void> insertDataPacket(DataPacket packet) async {
    final db = await instance.database;
    await db.insert('data_packets', packet.toMap());
  }

  // This function parses the string and inserts it in the db
  Future<void> processAndInsertData(String incomingData) async {
    var data = incomingData.split(',');

    if (data.length == 12) {
      var packet = DataPacket(
        epochTime: double.parse(data[0]),
        co2: double.parse(data[1]),
        ppm1_0: double.parse(data[2]),
        ppm2_5: double.parse(data[3]),
        ppm4_0: double.parse(data[4]),
        ppm10_0: double.parse(data[5]),
        humid: double.parse(data[6]),
        temp: double.parse(data[7]),
        voc: double.parse(data[8]),
        co: double.parse(data[9]),
        ng: double.parse(data[10]),
        aqi: double.parse(data[11]),
      );

      // Insert your data packet into the database
      await DatabaseService.instance.insertDataPacket(packet);
      // Implement mechanism to stop receiving data if required
    }
  }

  DataPacket parseFromMap(Map<String, double> map) {
    DataPacket data = DataPacket(
        epochTime: map["epochTime"]!,
        co2: map["co2"]!,
        ppm1_0: map["ppm1"]!,
        ppm2_5: map["ppm2p5"]!,
        ppm4_0: map["ppm4"]!,
        ppm10_0: map["ppm10"]!,
        humid: map["humid"]!,
        temp: map["temp"]!,
        voc: map["voc"]!,
        co: map["co"]!,
        ng: map["ng"]!,
        aqi: map["aqi"]!);

    return data;
  }

  Future<DataPacket?> getLastPacket() async {
    final db = await DatabaseService.instance.database;
    final List<Map<String, dynamic>> result = await db.query(
      'data_packets',
      orderBy: 'epochTime DESC',
      limit: 1,
    );

    if (result.isNotEmpty) {
      return DataPacket.fromMap(result.first);
    }
    return null;
  }
}
