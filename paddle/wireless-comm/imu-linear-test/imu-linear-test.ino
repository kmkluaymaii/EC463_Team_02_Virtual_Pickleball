#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

Adafruit_BNO055 bno(55);

uint32_t lastT = 0;

imu::Vector<3> vel(0,0,0);
imu::Vector<3> pos(0,0,0);
imu::Vector<3> accelBias(0,0,0);

const float damping = 0.98f;      // velocity drift control
const float stillThresh = 0.15f;  // m/s² threshold for bias update
const float biasLerp = 0.01f;     // slow-running bias accumulator

void setup() {
  Serial.begin(115200);
  if (!bno.begin()) {
    Serial.println("BNO055 not detected!");
    while (1);
  }
  delay(1000);
  bno.setExtCrystalUse(true);
}

void loop() {
  uint32_t now = micros();
  if (lastT == 0) {
    lastT = now;
    return;
  }

  float dt = (now - lastT) / 1e6f;   // seconds
  if (dt < 0.005f) return;           // ~200 Hz max
  lastT = now;

  // ---- READ SENSOR ----
  imu::Quaternion q = bno.getQuat();
  imu::Vector<3> a_body = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  // ---- ROTATE TO WORLD FRAME ----
  imu::Quaternion aq(0, a_body.x(), a_body.y(), a_body.z());
  imu::Quaternion q_conj(q.w(), -q.x(), -q.y(), -q.z());
  imu::Quaternion a_w_q = q * aq * q_conj;

  imu::Vector<3> a_world(a_w_q.x(), a_w_q.y(), a_w_q.z());

  // ---- BIAS ESTIMATION (WHEN STILL) ----
  if (a_world.magnitude() < stillThresh) {
    accelBias = accelBias * (1.0f - biasLerp) + a_world * biasLerp;
  }

  imu::Vector<3> a = a_world - accelBias;

  // ---- INTEGRATE ----
  vel += a * dt;
  vel *= damping;    // reduce drift

  pos += vel * dt;

  // ---- PRINT SERIAL ----
  Serial.print("Q: ");
  Serial.print(q.w(), 6); Serial.print(",");
  Serial.print(q.x(), 6); Serial.print(",");
  Serial.print(q.y(), 6); Serial.print(",");
  Serial.print(q.z(), 6);

  Serial.print(" | Pos: ");
  Serial.print(pos.x(), 4); Serial.print(",");
  Serial.print(pos.y(), 4); Serial.print(",");
  Serial.print(pos.z(), 4);

  Serial.print(" | Vel: ");
  Serial.print(vel.x(), 4); Serial.print(",");
  Serial.print(vel.y(), 4); Serial.print(",");
  Serial.print(vel.z(), 4);

  Serial.print(" | AccelW: ");
  Serial.print(a.x(), 4); Serial.print(",");
  Serial.print(a.y(), 4); Serial.print(",");
  Serial.println(a.z(), 4);
}