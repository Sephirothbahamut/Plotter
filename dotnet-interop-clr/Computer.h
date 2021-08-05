#pragma once

#include <string>
#include <memory>
#include <optional>

#ifdef DLL_EXPORT
#define DLL_API _declspec(dllexport)
#else
#define DLL_API _declspec(dllimport)
#endif

enum class sensor_type { voltage, clock, temperature, load, frequency, fan, flow, control, level, factor, power, data, smalldata, throughput, unknown };

class DLL_API Sensor
    {
    public:
        virtual std::string name() const noexcept = 0;
        virtual std::string identifier() const noexcept = 0;
        virtual sensor_type type() const noexcept = 0;
        virtual std::optional<float> value() const noexcept = 0;
        virtual std::string to_string() const noexcept = 0;
        virtual std::string to_string(uint8_t depth) const noexcept = 0;
    };

class DLL_API SensorMissing : public Sensor
    {
    virtual std::string name() const noexcept override { return "Missing sensor"; };
    virtual std::string identifier() const noexcept override { return "Missing sensor"; };
    virtual sensor_type type() const noexcept override { return sensor_type::unknown; };
    virtual std::optional<float> value() const noexcept override { return 0; };
    virtual std::string to_string() const noexcept override { return "Missing sensor"; };
    virtual std::string to_string(uint8_t depth) const noexcept override { return "Missing sensor"; };
    };

class DLL_API SensorSum : public Sensor
    {
    private:
        Sensor* s1;
        Sensor* s2;

    public:
        SensorSum(Sensor* s1, Sensor* s2) : s1(s1), s2(s2) {};

        virtual std::string name() const noexcept override { return "<" + s1->name() + "|" + s2->name() + ">"; };
        virtual std::string identifier() const noexcept override { return s1->identifier() + "|" + s2->identifier(); };;
        virtual sensor_type type() const noexcept override { return s1->type(); };
        virtual std::optional<float> value() const noexcept override
            {
            auto v1 = s1->value(), v2 = s2->value();
            return (v1.has_value() || v2.has_value()) ? std::optional<float>(v1.value_or(0) + v2.value_or(0)) : std::nullopt;
            }

        virtual std::string to_string() const noexcept override { return name() + " = {" + s1->to_string() + " <merged with>" + s2->to_string() + "}"; };
        virtual std::string to_string(uint8_t depth) const noexcept override;

        friend class Computer;
        friend class Hardware;
        friend class impl;
    };

class DLL_API SensorDirect : public Sensor
    {
    private:
        class impl;
        std::unique_ptr<impl> impl_;

    public:
        SensorDirect(const impl& inner);
        ~SensorDirect();

        virtual std::string name() const noexcept override;
        virtual std::string identifier() const noexcept override;
        virtual sensor_type type() const noexcept override;
        virtual std::optional<float> value() const noexcept override;
        virtual std::string to_string() const noexcept override;
        virtual std::string to_string(uint8_t depth) const noexcept override;

        friend class Computer;
        friend class Hardware;
        friend class impl;
    };

class DLL_API Hardware
    {
    private:
        class impl;
        std::unique_ptr<impl> impl_;

    public:
        Hardware(const impl& inner);
        ~Hardware();

        std::string name() const noexcept;
        std::string identifier() const noexcept;

        void update() noexcept;
        std::string to_string() const noexcept;
        std::string to_string(uint8_t depth) const noexcept;

        friend class Computer;
        friend class impl;
    };

class DLL_API Computer
    {
    private:
        class impl;
        std::unique_ptr<impl> impl_;

    public:
        Computer();
        ~Computer();

        void update() noexcept;
        std::string to_string() const noexcept;
        std::unique_ptr<Sensor> get_sensor(const std::string& identifier) const noexcept;
        std::unique_ptr<Hardware> get_hardware(const std::string& identifier) const noexcept;
        std::string get_name(const std::string& identifier) const noexcept;
    };

DLL_API void Print();