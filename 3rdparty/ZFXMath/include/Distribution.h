namespace ZFXMath
{

	template<typename T>
	struct NormalGaussianDistribution
	{
		T mean;
		T standardDeviation;

		NormalGaussianDistribution(const T mean, const T standardDeviation)
			: mean(mean)
			, standardDeviation(standardDeviation)
		{
		}

		template <int N>
		double EvaluatePolynomial(const double z, const double coefficients[N]) const
		{
			double sum = coefficients[N - 1];
			for (int i = N - 2; i >= 0; --i)
			{
				sum *= z;
				sum += coefficients[i];
			}

			return sum;
		}

		/// <summary>
		/// Implementation of the error function.
		/// </summary>
		/// <param name="z">Where to evaluate the error function.</param>
		/// <param name="invert">Whether to compute 1 - the error function.</param>
		/// <returns>the error function.</returns>
		double ErfImp(double z, bool invert) const
		{
			if (z < 0)
			{
				if (!invert)
				{
					return -ErfImp(-z, false);
				}

				if (z < -0.5)
				{
					return 2 - ErfImp((-z), true);
				}

				return 1 + ErfImp(-z, false);
			}

			double result;

			//
			// Big bunch of selection statements now to pick which
			// implementation to use, try to put most likely options
			// first:
			//
			if (z < 0.5)
			{
				//
				// We're going to calculate erf:
				//
				if (z < 1e-10)
				{
					result = (z*1.125) + (z*0.003379167095512573896158903121545171688);
				}
				else
				{
					// Worst case absolute error found: 6.688618532e-21
					const double nc[] = { 0.00337916709551257388990745, -0.00073695653048167948530905, -0.374732337392919607868241, 0.0817442448733587196071743, -0.0421089319936548595203468, 0.0070165709512095756344528, -0.00495091255982435110337458, 0.000871646599037922480317225 };
					const double dc[] = { 1, -0.218088218087924645390535, 0.412542972725442099083918, -0.0841891147873106755410271, 0.0655338856400241519690695, -0.0120019604454941768171266, 0.00408165558926174048329689, -0.000615900721557769691924509 };

					result = (z*1.125) + (z*EvaluatePolynomial<8>(z, nc) / EvaluatePolynomial<8>(z, dc));
				}
			}
			else if ((z < 110) || ((z < 110) && invert))
			{
				//
				// We'll be calculating erfc:
				//
				invert = !invert;
				double r, b;
				if (z < 0.75)
				{
					// Worst case absolute error found: 5.582813374e-21
					const double nc[] = { -0.0361790390718262471360258, 0.292251883444882683221149, 0.281447041797604512774415, 0.125610208862766947294894, 0.0274135028268930549240776, 0.00250839672168065762786937 };
					const double dc[] = { 1, 1.8545005897903486499845, 1.43575803037831418074962, 0.582827658753036572454135, 0.124810476932949746447682, 0.0113724176546353285778481 };
					r = EvaluatePolynomial<6>(z - 0.5, nc) / EvaluatePolynomial<6>(z - 0.5, dc);
					b = 0.3440242112F;
				}
				else if (z < 1.25)
				{
					// Worst case absolute error found: 4.01854729e-21
					const double nc[] = { -0.0397876892611136856954425, 0.153165212467878293257683, 0.191260295600936245503129, 0.10276327061989304213645, 0.029637090615738836726027, 0.0046093486780275489468812, 0.000307607820348680180548455 };
					const double dc[] = { 1, 1.95520072987627704987886, 1.64762317199384860109595, 0.768238607022126250082483, 0.209793185936509782784315, 0.0319569316899913392596356, 0.00213363160895785378615014 };
					r = EvaluatePolynomial<7>(z - 0.75, nc) / EvaluatePolynomial<7>(z - 0.75, dc);
					b = 0.419990927F;
				}
				else if (z < 2.25)
				{
					// Worst case absolute error found: 2.866005373e-21
					const double nc[] = { -0.0300838560557949717328341, 0.0538578829844454508530552, 0.0726211541651914182692959, 0.0367628469888049348429018, 0.00964629015572527529605267, 0.00133453480075291076745275, 0.778087599782504251917881e-4 };
					const double dc[] = { 1, 1.75967098147167528287343, 1.32883571437961120556307, 0.552528596508757581287907, 0.133793056941332861912279, 0.0179509645176280768640766, 0.00104712440019937356634038, -0.106640381820357337177643e-7 };
					r = EvaluatePolynomial<7>(z - 1.25, nc) / EvaluatePolynomial<8>(z - 1.25, dc);
					b = 0.4898625016F;
				}
				else if (z < 3.5)
				{
					// Worst case absolute error found: 1.045355789e-21
					const double nc[] = { -0.0117907570137227847827732, 0.014262132090538809896674, 0.0202234435902960820020765, 0.00930668299990432009042239, 0.00213357802422065994322516, 0.00025022987386460102395382, 0.120534912219588189822126e-4 };
					const double dc[] = { 1, 1.50376225203620482047419, 0.965397786204462896346934, 0.339265230476796681555511, 0.0689740649541569716897427, 0.00771060262491768307365526, 0.000371421101531069302990367 };
					r = EvaluatePolynomial<7>(z - 2.25, nc) / EvaluatePolynomial<7>(z - 2.25, dc);
					b = 0.5317370892F;
				}
				else if (z < 5.25)
				{
					// Worst case absolute error found: 8.300028706e-22
					const double nc[] = { -0.00546954795538729307482955, 0.00404190278731707110245394, 0.0054963369553161170521356, 0.00212616472603945399437862, 0.000394984014495083900689956, 0.365565477064442377259271e-4, 0.135485897109932323253786e-5 };
					const double dc[] = { 1, 1.21019697773630784832251, 0.620914668221143886601045, 0.173038430661142762569515, 0.0276550813773432047594539, 0.00240625974424309709745382, 0.891811817251336577241006e-4, -0.465528836283382684461025e-11 };
					r = EvaluatePolynomial<7>(z - 3.5, nc) / EvaluatePolynomial<8>(z - 3.5, dc);
					b = 0.5489973426F;
				}
				else if (z < 8)
				{
					// Worst case absolute error found: 1.700157534e-21
					const double nc[] = { -0.00270722535905778347999196, 0.0013187563425029400461378, 0.00119925933261002333923989, 0.00027849619811344664248235, 0.267822988218331849989363e-4, 0.923043672315028197865066e-6 };
					const double dc[] = { 1, 0.814632808543141591118279, 0.268901665856299542168425, 0.0449877216103041118694989, 0.00381759663320248459168994, 0.000131571897888596914350697, 0.404815359675764138445257e-11 };
					r = EvaluatePolynomial<6>(z - 5.25, nc) / EvaluatePolynomial<7>(z - 5.25, dc);
					b = 0.5571740866F;
				}
				else if (z < 11.5)
				{
					// Worst case absolute error found: 3.002278011e-22
					const double nc[] = { -0.00109946720691742196814323, 0.000406425442750422675169153, 0.000274499489416900707787024, 0.465293770646659383436343e-4, 0.320955425395767463401993e-5, 0.778286018145020892261936e-7 };
					const double dc[] = { 1, 0.588173710611846046373373, 0.139363331289409746077541, 0.0166329340417083678763028, 0.00100023921310234908642639, 0.24254837521587225125068e-4 };
					r = EvaluatePolynomial<6>(z - 8, nc) / EvaluatePolynomial<6>(z - 8, dc);
					b = 0.5609807968F;
				}
				else if (z < 17)
				{
					// Worst case absolute error found: 6.741114695e-21
					const double nc[] = { -0.00056907993601094962855594, 0.000169498540373762264416984, 0.518472354581100890120501e-4, 0.382819312231928859704678e-5, 0.824989931281894431781794e-7 };
					const double dc[] = { 1, 0.339637250051139347430323, 0.043472647870310663055044, 0.00248549335224637114641629, 0.535633305337152900549536e-4, -0.117490944405459578783846e-12 };
					r = EvaluatePolynomial<5>(z - 11.5, nc) / EvaluatePolynomial<6>(z - 11.5, dc);
					b = 0.5626493692F;
				}
				else if (z < 24)
				{
					// Worst case absolute error found: 7.802346984e-22
					const double nc[] = { -0.000241313599483991337479091, 0.574224975202501512365975e-4, 0.115998962927383778460557e-4, 0.581762134402593739370875e-6, 0.853971555085673614607418e-8 };
					const double dc[] = { 1, 0.233044138299687841018015, 0.0204186940546440312625597, 0.000797185647564398289151125, 0.117019281670172327758019e-4 };
					r = EvaluatePolynomial<5>(z - 17, nc) / EvaluatePolynomial<5>(z - 17, dc);
					b = 0.5634598136F;
				}
				else if (z < 38)
				{
					// Worst case absolute error found: 2.414228989e-22
					const double nc[] = { -0.000146674699277760365803642, 0.162666552112280519955647e-4, 0.269116248509165239294897e-5, 0.979584479468091935086972e-7, 0.101994647625723465722285e-8 };
					const double dc[] = { 1, 0.165907812944847226546036, 0.0103361716191505884359634, 0.000286593026373868366935721, 0.298401570840900340874568e-5 };
					r = EvaluatePolynomial<5>(z - 24, nc) / EvaluatePolynomial<5>(z - 24, dc);
					b = 0.5638477802F;
				}
				else if (z < 60)
				{
					// Worst case absolute error found: 5.896543869e-24
					const double nc[] = { -0.583905797629771786720406e-4, 0.412510325105496173512992e-5, 0.431790922420250949096906e-6, 0.993365155590013193345569e-8, 0.653480510020104699270084e-10 };
					const double dc[] = { 1, 0.105077086072039915406159, 0.00414278428675475620830226, 0.726338754644523769144108e-4, 0.477818471047398785369849e-6 };
					r = EvaluatePolynomial<5>(z - 38, nc) / EvaluatePolynomial<5>(z - 38, dc);
					b = 0.5640528202F;
				}
				else if (z < 85)
				{
					// Worst case absolute error found: 3.080612264e-21
					const double nc[] = { -0.196457797609229579459841e-4, 0.157243887666800692441195e-5, 0.543902511192700878690335e-7, 0.317472492369117710852685e-9 };
					const double dc[] = { 1, 0.052803989240957632204885, 0.000926876069151753290378112, 0.541011723226630257077328e-5, 0.535093845803642394908747e-15 };
					r = EvaluatePolynomial<4>(z - 60, nc) / EvaluatePolynomial<5>(z - 60, dc);
					b = 0.5641309023F;
				}
				else
				{
					// Worst case absolute error found: 8.094633491e-22
					const double nc[] = { -0.789224703978722689089794e-5, 0.622088451660986955124162e-6, 0.145728445676882396797184e-7, 0.603715505542715364529243e-10 };
					const double dc[] = { 1, 0.0375328846356293715248719, 0.000467919535974625308126054, 0.193847039275845656900547e-5 };
					r = EvaluatePolynomial<4>(z - 85, nc) / EvaluatePolynomial<4>(z - 85, dc);
					b = 0.5641584396F;
				}

				double g = exp(-z*z) / z;
				result = (g*b) + (g*r);
			}
			else
			{
				//
				// Any value of z larger than 28 will underflow to zero:
				//
				result = 0;
				invert = !invert;
			}

			if (invert)
			{
				result = 1 - result;
			}

			return result;
		}


		T Erfc(const T x) const
		{
			if (x == 0)
			{
				return 1;
			}

			return (T)ErfImp((double)x, true);
		}

		T Density(T x) const
		{
			const T Sqrt2Pi = (T)2.5066282746310005024157652848110452530069867406099;
			auto d = (x - mean) / standardDeviation;
			return exp(-0.5*d*d) / (Sqrt2Pi * standardDeviation);
		}

		T CumulativeDistribution(T x) const
		{
			return (T)0.5 * Erfc((mean - x) / (standardDeviation * SQRT_2));
		}
	};


	// http://www.itl.nist.gov/div898/handbook/eda/section3/eda35e.htm
	template<typename T, typename D>
	T ComputeAndersonDarlingTestStatistic(const T* distribution, const int numValues, const D& referenceDistribution)
	{
		int n = numValues;
		T sum = (T)0.0;

		for (int i = 1; i <= n; i++)
		{
			sum += (T)(2 * i - 1) * ((T)log(referenceDistribution.CumulativeDistribution(distribution[i - 1])) + (T)log((T)1.0 - referenceDistribution.CumulativeDistribution(distribution[n - i])));
		}

		T A2 = ((T)-1.0 / (T)n) * sum - (T)n;

		// correct A2 for unknown mean and unknown variance
		A2 *= (T)1.0 + (T)4.0 / (T)n - (T)25.0 / (T)(n * n);

		return A2;
	}

	template< int N, typename T >
	struct MultivariateGaussianDistribution // http://en.wikipedia.org/wiki/Multivariate_normal_distribution
	{
	private:

		bool isValid;
		bool normalizeDensity;
		TMatrixMxN<N, N, T> invCovariance;
		T inverseDensityScale;

	public:
		TMatrixMxN<N, N, T> covarianceMatrix;
		TVectorN<N, T> meanVector;
		T variance;

		MultivariateGaussianDistribution()
			: isValid(false)
			, meanVector((T)0.0)
			, variance((T)0.0)
		{
		}

		bool IsValid() const
		{
			return isValid;
		}

		void ComputeProbabilityDensityFunction(const TVectorN<N, T>* samples, const size_t numSamples, const bool normalizeDensity = true)
		{
			this->normalizeDensity = normalizeDensity;

			covarianceMatrix.ComputeCovarianceMatrix(meanVector, samples, numSamples);

			UpdateProbabilityDensityFunction(covarianceMatrix, normalizeDensity);
		}

		void UpdateProbabilityDensityFunction(const TMatrixMxN<N, N, T>& covarianceMatrix, const bool normalizeDensity)
		{
			variance = (T)0.0;
			for (size_t n = 0; n < N; n++)
			{
				variance += covarianceMatrix.matrix[n][n];
			}

			double covarianceDeterminant;
			isValid = covarianceMatrix.ComputeInverseMatrix(invCovariance, covarianceDeterminant);
			if (isValid)
			{
				const double densityScaleSquared = pow(2.0 * PI, N) * covarianceDeterminant;
				inverseDensityScale = 1.0 / sqrt(densityScaleSquared);

				if (normalizeDensity)
				{
					const double ellipsoidVolume = covarianceMatrix.ComputeEllipsoidVolume();
					inverseDensityScale *= ellipsoidVolume; // we multiply volume here, to get comparable densities among different MVGDs
				}
			}
		}

		T EvaluateEuclideanDistanceSquared(const TVectorN<N, T>& sample) const
		{
			return (meanVector - sample).LengthSqr();
		}

		T EvaluateMahalanobisDistanceSquared(const TVectorN<N, T>& sample) const
		{
			const auto& x = sample;
			const auto& mu = meanVector;
			const auto& S1 = invCovariance;

			const auto mahalanobisDistanceSquared = (x - mu).DotProduct(S1 * (x - mu));
			return mahalanobisDistanceSquared;
		}

		T EvaluateDensity(const TVectorN<N, T>& sample) const
		{
			const auto mahalanobisDistanceSquared = EvaluateMahalanobisDistanceSquared(sample);
			const auto density = ::exp(-0.5 * mahalanobisDistanceSquared) * inverseDensityScale;

			return density;
		}

		void ScaleDensityFunction(const TVectorN<N, T>& dir, const T scale)
		{
			assert(N == 2);

			T dirAngle = -ATan2(dir.val[1], dir.val[0]);
			auto invRotation = TMatrix3x3<double>::Rotation2D(-dirAngle);
			auto rotation = TMatrix3x3<double>::Rotation2D(dirAngle);
			auto scaleM = TMatrix3x3<double>::Identity().Scale(scale, 1.0, 0.0);

			TMatrix3x3<double> cvm(covarianceMatrix.matrix[0][0], covarianceMatrix.matrix[0][1], 0.0,
				covarianceMatrix.matrix[1][0], covarianceMatrix.matrix[1][1], 0.0, 0.0, 0.0, 0.0);

			TMatrix3x3<double> scm3 = ((cvm * invRotation) * scaleM) * rotation;

			TMatrixMxN<N, N, T> scaledCovarianceMatrix;
			scaledCovarianceMatrix.matrix[0][0] = scm3.matrix[0][0];
			scaledCovarianceMatrix.matrix[0][1] = scm3.matrix[0][1];
			scaledCovarianceMatrix.matrix[1][0] = scm3.matrix[1][0];
			scaledCovarianceMatrix.matrix[1][1] = scm3.matrix[1][1];

			UpdateProbabilityDensityFunction(scaledCovarianceMatrix, normalizeDensity);
		}

		TVectorN<N, T> ComputePrincipalComponentVector(T* eigenValueOut = NULL)
		{
			T lengthSqr = 0.0;
			TVectorN<N, T> pcv = TVectorN<N, T>(0.0);
			for (int n = 0; n < N; n++)
			{
				TVectorN<N, T> compVector = covarianceMatrix.GetCol(n);

				T compLengthSqr = compVector.LengthSqr();
				if (lengthSqr < compLengthSqr)
				{
					lengthSqr = compLengthSqr;
					pcv = compVector;
					if (eigenValueOut)
					{
						*eigenValueOut = covarianceMatrix[n][n];
					}
				}
			}

			return pcv;
		}
	};

}
