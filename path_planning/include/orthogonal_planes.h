#ifndef ORTHOGONAL_PLANES_H
#define ORTHOGONAL_PLANES_H

#include <octomap/math/Vector3.h>
#include <ltStarOctree_common.h>
#include <ros/ros.h>

#include <Eigen/Dense>


namespace LazyThetaStarOctree{

	class CoordinateFrame
	{
	public:
		const octomath::Vector3 direction;
		const octomath::Vector3 orthogonalA;
		const octomath::Vector3 orthogonalB;
		CoordinateFrame (octomath::Vector3 direction, octomath::Vector3 orthogonalA, octomath::Vector3 orthogonalB)
			: direction(direction), orthogonalA(orthogonalA), orthogonalB(orthogonalB)
			{}
	};

	CoordinateFrame generateCoordinateFrame(octomath::Vector3 const& start, octomath::Vector3 const& goal)
	{
		const octomath::Vector3 xAxis (1, 0, 0);
		const octomath::Vector3 zAxis (0, 0, 1);
		const octomath::Vector3 zero  (0, 0, 0);
		octomath::Vector3 oA, oB;


		octomath::Vector3 direction = goal - start;
		direction.normalize();

		if(  std::abs( direction.dot(zAxis) ) <= 0.9 )
		{
			oA = zAxis.cross(direction);
		}
		else
		{
			oA = direction.cross(xAxis);
		}

		oA.normalize();

		// ROS_WARN_STREAM("direction: " << direction);
		// ROS_WARN_STREAM("orthogonalA: " << oA);
		// ROS_WARN_STREAM(direction << ".dot(" << oA << ") =  " << direction.dot(oA));

		oB = direction.cross(oA);
		// ROS_WARN_STREAM("orthogonalB: " << oB);


		CoordinateFrame coord (direction, oA, oB);
		return coord;
	}

	octomath::Vector3 calculateGoalWithMargin(octomath::Vector3 const& start, octomath::Vector3 const& goal, double margin)
	{
		octomath::Vector3 direction = goal - start;
		double distance = direction.norm();
		direction.normalize();
		octomath::Vector3 scale = direction * (margin/2);
		return goal + scale;
	}

	octomath::Vector3 rotateAndTranslate(CoordinateFrame coordinate_frame, octomath::Vector3 offsetPoint, octomath::Vector3 start, octomath::Vector3 goalWithMargin)
	{
		Eigen::MatrixXd m(3, 3);
		m(0, 0) = coordinate_frame.direction.x();
		m(1, 0) = coordinate_frame.direction.y();
		m(2, 0) = coordinate_frame.direction.z();

		m(0, 1) = coordinate_frame.orthogonalA.x();
		m(1, 1) = coordinate_frame.orthogonalA.y();
		m(2, 1) = coordinate_frame.orthogonalA.z();

		m(0, 2) = coordinate_frame.orthogonalB.x();
		m(1, 2) = coordinate_frame.orthogonalB.y();
		m(2, 2) = coordinate_frame.orthogonalB.z();
		// Matrix r = (direction.x, orthogonalA.x, orthogonalB.x )
		//            (direction.y, orthogonalA.y, orthogonalB.y )
		//            (direction.z, orthogonalA.z, orthogonalB.z )

		ROS_WARN_STREAM(m);
		// rotated_point = r * offsetPoint;
		// rotated_start = rotated_point + start;
		// rotated_goalWithMargin  = rotated_point + goalWithMargin ;
	}


	void generateRectanglePlaneIndexes(double margin, double resolution, std::vector<octomath::Vector3> & plane)
	{
	}

	void generateCirclePlaneIndexes(double margin, double resolution, std::vector<octomath::Vector3> & plane)
	{
		int n = margin/ resolution;
		ROS_WARN_STREAM("N " << n);
		int loop_count =   n * 2  + 1;
		int array_index = 0;
		double  x, y, y_, z_;
		x = y = y_ = z_ = 0;

		double rectSquare = ( margin / resolution ) * ( margin / resolution );

		ROS_WARN_STREAM("rectSquare " << rectSquare);

		for (int i = 0; i < loop_count; ++i)
		{
			x = std::abs (i - n) - 0.5;
			for (int j = 0; j < loop_count; ++j)
			{
				y = std::abs(j - n) - 0.5;

				if( x*x + y*y  <= rectSquare )
				{
					y_ = (i - n) * resolution;
					z_ = (j - n) * resolution;
					plane.emplace(plane.end(), 0, y_, z_);
					array_index++;
				}
			}
		}
	}

	void generateSemiSphereOut(double margin, double resolution, std::vector<octomath::Vector3> & plane, std::vector<octomath::Vector3> & semiSphere)
	{
		double depth;
		double safety_range = margin + 0.5 * resolution;
		for (std::vector<octomath::Vector3>::iterator i = plane.begin(); i != plane.end(); ++i)
		{
			depth = std::sqrt( safety_range * safety_range  -  i->y()*i->y()  -   i->z()*i->z());
			semiSphere.emplace(semiSphere.end(), depth, i->y(), i->z());
		}
	} 

	void generateSemiSphereIn(double margin, double resolution, std::vector<octomath::Vector3> & plane, std::vector<octomath::Vector3> & semiSphere)
	{
		double depth;
		double safety_range = margin + 0.5 * resolution;
		for (std::vector<octomath::Vector3>::iterator i = plane.begin(); i != plane.end(); ++i)
		{
			depth = std::sqrt( safety_range * safety_range  -  i->y()*i->y()  -   i->z()*i->z());
			semiSphere.emplace(semiSphere.end(), -depth, i->y(), i->z());
		}
	} 

}

#endif // ORTHOGONAL_PLANES_H