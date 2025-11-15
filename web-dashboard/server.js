const express = require('express');
const axios = require('axios');
const cors = require('cors');
const compression = require('compression');
const http = require('http');
const { Server } = require('socket.io');
require('dotenv').config();

const app = express();
const server = http.createServer(app);
const io = new Server(server);
const PORT = process.env.PORT || 3000;

// ===== THINGSPEAK CONFIG =====
const CHANNEL_ID = process.env.THINGSPEAK_CHANNEL_ID || 'YOUR_CHANNEL_ID';
const READ_API_KEY = process.env.THINGSPEAK_READ_API_KEY || 'YOUR_READ_API_KEY';
const THINGSPEAK_BASE_URL = 'https://api.thingspeak.com/channels';

// ===== MIDDLEWARE =====
app.use(cors());
app.use(express.json());
app.use(compression());
app.use(express.static('public'));

// ===== LOGGING MIDDLEWARE =====
app.use((req, res, next) => {
  const timestamp = new Date().toISOString();
  console.log(`[${timestamp}] ${req.method} ${req.path}`);
  next();
});

// ===== HELPER FUNCTIONS =====
function parseThingSpeakFeed(feed) {
  return {
    soilMoisture: parseFloat(feed.field1) || 0,
    temperature: parseFloat(feed.field2) || 0,
    humidity: parseFloat(feed.field3) || 0,
    pumpStatus: parseInt(feed.field4) || 0,
    timestamp: feed.created_at
  };
}

// ===== WEBSOCKET CONNECTION =====
io.on('connection', (socket) => {
  console.log('🔌 Client connected:', socket.id);
  
  socket.on('disconnect', () => {
    console.log('🔌 Client disconnected:', socket.id);
  });
});

// ===== AUTO PUSH DATA VIA WEBSOCKET =====
setInterval(async () => {
  try {
    const url = `${THINGSPEAK_BASE_URL}/${CHANNEL_ID}/feeds.json`;
    const response = await axios.get(url, {
      params: { api_key: READ_API_KEY, results: 1 },
      timeout: 5000
    });

    if (response.data.feeds && response.data.feeds.length > 0) {
      const data = parseThingSpeakFeed(response.data.feeds[0]);
      io.emit('sensorData', data);  // Push to all clients
    }
  } catch (error) {
    console.error('❌ WebSocket push error:', error.message);
  }
}, 3000);  // Push mỗi 3 giây

// ===== API ROUTES =====

// Health check
app.get('/api/health', (req, res) => {
  res.json({
    status: 'OK',
    timestamp: new Date().toISOString(),
    uptime: process.uptime()
  });
});

// Lấy dữ liệu mới nhất
app.get('/api/latest', async (req, res) => {
  try {
    const url = `${THINGSPEAK_BASE_URL}/${CHANNEL_ID}/feeds.json`;
    const response = await axios.get(url, {
      params: {
        api_key: READ_API_KEY,
        results: 1
      },
      timeout: 5000
    });

    if (response.data.feeds && response.data.feeds.length > 0) {
      const data = parseThingSpeakFeed(response.data.feeds[0]);
      res.json({
        success: true,
        data: data,
        channel: {
          id: response.data.channel.id,
          name: response.data.channel.name,
          lastUpdate: response.data.channel.updated_at
        }
      });
    } else {
      res.json({
        success: false,
        message: 'Không có dữ liệu',
        data: null
      });
    }
  } catch (error) {
    console.error('❌ Lỗi API /latest:', error.message);
    res.status(500).json({
      success: false,
      message: 'Lỗi kết nối ThingSpeak: ' + error.message
    });
  }
});

// Lấy lịch sử dữ liệu
app.get('/api/history', async (req, res) => {
  try {
    const results = parseInt(req.query.results) || 100;
    const url = `${THINGSPEAK_BASE_URL}/${CHANNEL_ID}/feeds.json`;
    
    const response = await axios.get(url, {
      params: {
        api_key: READ_API_KEY,
        results: Math.min(results, 8000) // Max 8000 records
      },
      timeout: 10000
    });

    if (response.data.feeds) {
      const feeds = response.data.feeds.map(parseThingSpeakFeed);
      res.json({
        success: true,
        data: feeds,
        count: feeds.length
      });
    } else {
      res.json({
        success: false,
        message: 'Không có dữ liệu lịch sử'
      });
    }
  } catch (error) {
    console.error('❌ Lỗi API /history:', error.message);
    res.status(500).json({
      success: false,
      message: 'Lỗi server: ' + error.message
    });
  }
});

// Thống kê dữ liệu
app.get('/api/stats', async (req, res) => {
  try {
    const url = `${THINGSPEAK_BASE_URL}/${CHANNEL_ID}/feeds.json`;
    const response = await axios.get(url, {
      params: {
        api_key: READ_API_KEY,
        results: 200
      },
      timeout: 10000
    });

    if (response.data.feeds && response.data.feeds.length > 0) {
      const feeds = response.data.feeds;

      // Lọc dữ liệu hợp lệ
      const soilValues = feeds.map(f => parseFloat(f.field1)).filter(v => v > 0);
      const tempValues = feeds.map(f => parseFloat(f.field2)).filter(v => v > 0);
      const humValues = feeds.map(f => parseFloat(f.field3)).filter(v => v > 0);
      const pumpEvents = feeds.filter(f => parseInt(f.field4) === 1).length;

      // Tính toán
      const avgSoil = soilValues.length > 0 
        ? (soilValues.reduce((a, b) => a + b, 0) / soilValues.length).toFixed(1) 
        : 0;
      const avgTemp = tempValues.length > 0 
        ? (tempValues.reduce((a, b) => a + b, 0) / tempValues.length).toFixed(1) 
        : 0;
      const avgHum = humValues.length > 0 
        ? (humValues.reduce((a, b) => a + b, 0) / humValues.length).toFixed(1) 
        : 0;

      res.json({
        success: true,
        stats: {
          avgSoilMoisture: parseFloat(avgSoil),
          avgTemperature: parseFloat(avgTemp),
          avgHumidity: parseFloat(avgHum),
          minSoilMoisture: soilValues.length > 0 ? Math.min(...soilValues).toFixed(1) : 0,
          maxSoilMoisture: soilValues.length > 0 ? Math.max(...soilValues).toFixed(1) : 0,
          totalReadings: feeds.length,
          pumpActivations: pumpEvents,
          dataRange: {
            from: feeds[feeds.length - 1]?.created_at || null,
            to: feeds[0]?.created_at || null
          }
        }
      });
    } else {
      res.json({
        success: false,
        message: 'Không đủ dữ liệu để thống kê'
      });
    }
  } catch (error) {
    console.error('❌ Lỗi API /stats:', error.message);
    res.status(500).json({
      success: false,
      message: 'Lỗi server: ' + error.message
    });
  }
});

// Lấy dữ liệu theo khoảng thời gian
app.get('/api/range', async (req, res) => {
  try {
    const { start, end } = req.query;
    
    if (!start || !end) {
      return res.status(400).json({
        success: false,
        message: 'Cần cung cấp start và end timestamp'
      });
    }

    const url = `${THINGSPEAK_BASE_URL}/${CHANNEL_ID}/feeds.json`;
    const response = await axios.get(url, {
      params: {
        api_key: READ_API_KEY,
        start: start,
        end: end
      },
      timeout: 10000
    });

    if (response.data.feeds) {
      const feeds = response.data.feeds.map(parseThingSpeakFeed);
      res.json({
        success: true,
        data: feeds,
        count: feeds.length
      });
    } else {
      res.json({
        success: false,
        message: 'Không có dữ liệu trong khoảng thời gian này'
      });
    }
  } catch (error) {
    console.error('❌ Lỗi API /range:', error.message);
    res.status(500).json({
      success: false,
      message: 'Lỗi server: ' + error.message
    });
  }
});

// 404 handler
app.use((req, res) => {
  res.status(404).json({
    success: false,
    message: 'API endpoint không tồn tại'
  });
});

// Error handler
app.use((err, req, res, next) => {
  console.error('💥 Server Error:', err);
  res.status(500).json({
    success: false,
    message: 'Lỗi server không xác định'
  });
});

// ===== START SERVER =====
app.listen(PORT, () => {
  console.log('\n╔════════════════════════════════════╗');
  console.log('║  🌱 SMART IRRIGATION DASHBOARD 🌱  ║');
  console.log('╚════════════════════════════════════╝\n');
  console.log(`🚀 Server đang chạy tại: http://localhost:${PORT}`);
  console.log(`📊 ThingSpeak Channel ID: ${CHANNEL_ID}`);
  console.log(`⏰ Started at: ${new Date().toLocaleString('vi-VN')}\n`);
  console.log('📡 API Endpoints:');
  console.log(`   GET /api/health   - Health check`);
  console.log(`   GET /api/latest   - Dữ liệu mới nhất`);
  console.log(`   GET /api/history  - Lịch sử dữ liệu`);
  console.log(`   GET /api/stats    - Thống kê`);
  console.log(`   GET /api/range    - Dữ liệu theo khoảng thời gian\n`);
});