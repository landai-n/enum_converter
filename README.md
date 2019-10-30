
# enum_converter
enum_converter is a secure C++17 enum converter

A main use case is to use it within the bridge design pattern. It allows to map bridge enum values with 
external interface's enum values.

It could also be used to associate std::string to enum values.

# Usage
Declare an enum converter:

    using Converter = lenewt::enum_converter<InternalEnum, ExternalEnum>;
Constructor

    const auto& converter = Converter { 
	    Converter::equiv{InternalEnum::ValueA, ExternalEnum::ValueA}, // set a bidirectional conversion mapping
	    ...
	    Converter::internal_proj{InternalEnum::ValueB, ExternalEnum::ValueOther}, // set a monodirectional conversion from InternalEnum to ExternalEnum
	    ....
	    Converter::external_proj{InternalEnum::ValueOther, ExternalEnum::ValueC}, // set a monodirectional conversion from ExternalEnum to InternalEnum
	    ...
    };
Cast enum values (Throw lenewt::enum_converter::undefined_enum_error if nothing match)

    converter.cast(InternalEnum::ValueA); // return mapped ExternalEnum value
    converter.cast(ExternalEnum::ValueB); // return mapped InternalEnum value
    
Cast enum values (return std::optional set to std::nulloprt if nothing match)

    converter.try_cast(InternalEnum::ValueA); // return mapped ExternalEnum value
    converter.try_cast(ExternalEnum::ValueB); // return mapped InternalEnum value
Used with strings equivalent

    using Converter = lenewt::enum_converter<Enum, std::string>;
    
    const auto& converter = Converter {
	    Converter::equiv{Enum::ValueA, "ValueA"},
	    Converter::equiv{Enum::ValueB, "ValueB"}
	  };

# Example 

    enum class InternalVehicle {
	    Car,
	    Bus,
	    Bike,
	    Unknown
    };
    
    enum class Vehicle {
	    Car,
	    ElectricCar,
	    Bus,
	    Bike,
	    Boat
    }l
    
    using VehicleConverter = lenewt::enum_converter<InternalVehicle, Vehicle>;
    
    const auto& vehicleConverter = VehicleConverter {
	    VehicleConverter::equiv{InternalVehicle::Car, Vehicle::Car},
	    VehicleConverter::equiv{InternalVehicle::Bus, Vehicle::Bus},
	    VehicleConverter::equiv{InternalVehicle::Bike, Vehicle::Bike},
	    VehicleConverter::internal_proj{InternalVehicle::Boat, Vehicle::Unknown},
	    VehicleConverter::external_proj{InternalVehicle::Car, Vehicle::ElectricCar}
    };
    
    Vehicle car = vehicleConverter.cast(InternalVehicle::Car); // Vehicle::Car
    std::optional<Vehicle> bikeOpt = vehicleConverter.try_cast(InternalVehicle::Bike); // Vehicle::Bike
    
    InternalVehicle internalCar = vehicleConverter.cast(Vehicle::ElectricCar); // InternalVehicle::car
    

